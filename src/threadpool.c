/**
 * This file is part of helios.
 * 
 * helios is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * helios is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with helios.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "threadpool.h"
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

enum _status {
  STOPPED = 0,
  RUNNABLE,
  RUNNING,
};

struct _worker {
  enum _status status; /* The status */
  void * (*func) (void *); /* The job function */
  void *arg; /* The job's argument */
  void *retval; /* The job's return value, or null */
  int has_data; /* Whether there's data to return */
  threadpool *parent; /* The threadpool that created this */
  pthread_t thread; /* The posix thread for this worker */
  int job_index; /* The index of the running job */
};

struct _threadpool {
  int count; /* How many threads? */
  int running; /* Am I running a job? */
  struct _worker *workers; /* The worker structs */
  pthread_mutex_t lock; /* The pool's lock */
  pthread_cond_t cv; /* The pool's cv - indicates change of thread state */
  int initializing; /* How many workers are initializing */
};

/**
 * The function that the worker threads run.
 */
static void *_worker_func(void *the_worker);

int threadpool_create(threadpool **retval, int threadcount) {
  assert(retval != NULL);
  threadpool *pool = malloc(sizeof(threadpool));
  if (pool == NULL) {
    return 0;
  }
  if (pthread_mutex_init(&(pool->lock), NULL)) {
    free(pool);
    return 0;
  }
  if (pthread_cond_init(&(pool->cv), NULL)) {
    free(pool);
    pthread_mutex_destroy(&(pool->lock));
    return 0;
  }
  pool->workers = calloc(threadcount, sizeof(struct _worker));
  if (pool->workers == NULL) {
    free(pool);
    pthread_cond_destroy(&(pool->cv));
    pthread_mutex_destroy(&(pool->lock));
    return 0;
  }
  pool->count = threadcount;
  pool->initializing = 0;
  pthread_mutex_lock(&(pool->lock));
  for (int i = 0; i < threadcount; i++) {
    struct _worker *w = &(pool->workers[i]);
    if(pthread_create(&(w->thread), NULL, _worker_func, (void *) w)) {
      /* TODO This is a delicate one and I don't feel like writing the
       * error handler right now. Deal with this later. It can go through
       * destroy as long as the locking situation is properly dealt with */
      return 0;
    }
    w->parent = pool;
    w->status = RUNNABLE;
    pool->initializing++;
  }
  while (pool->initializing) {
    pthread_cond_wait(&(pool->cv), &(pool->lock));
  }
  pthread_mutex_unlock(&(pool->lock));
  /* At this stage all the threads will collapse into their waits */
  *retval = pool;
  return 1;
}

int threadpool_destroy(threadpool *pool) {
  assert(pool != NULL);
  pthread_mutex_lock(&(pool->lock));
  while (pool->running) {
    pthread_cond_wait(&(pool->cv), &(pool->lock));
  }
  /* It's not supposed to be running any more jobs, but let's just make sure 
   * here aren't any runaway threads, then stop them one by one */
  int t;
  for (t = 0; t < pool->count; t++) {
    struct _worker *w = &(pool->workers[t]);
    while (w->status == RUNNING) {
      pthread_cond_wait(&(pool->cv), &(pool->lock));
    }
    w->status = STOPPED;
  }
  pthread_cond_broadcast(&(pool->cv));
  pthread_mutex_unlock(&(pool->lock));
  /* They're all gonna exit now, so just join them */
  for (t = 0; t < pool->count; t++) {
    struct _worker *w = &(pool->workers[t]);
    pthread_join(w->thread, NULL);
  }
  free(pool->workers);
  pthread_cond_destroy(&(pool->cv));
  pthread_mutex_destroy(&(pool->lock));
  free(pool);
  return 1;
}

int threadpool_submit(threadpool *pool, void **retvals,
    void * (*mapper)(void *), unsigned char *arguments, size_t arg_size,
    int arg_count) {
  int submitted = 0;
  int done = 0;
  int t = 0;
  pthread_mutex_lock(&(pool->lock));
  pool->running = 1;
  /* We're only gonna signal once per big loop, and we don't wanna do it
   * unless we've actually woken up a thread */
  int signal = 0;
  while (done < arg_count) {
    for (t = 0; t < pool->count; t++) {
      struct _worker *w = &(pool->workers[t]);
      if (w->status == RUNNABLE) {
        if (w->has_data) {
          if (retvals) {
            retvals[w->job_index] = w->retval;
          }
          w->has_data = 0;
          done++;
        }
        if (submitted < arg_count) {
          signal = 1;
          w->job_index = submitted;
          w->func = mapper;
          w->arg = (void *) (arguments + (submitted * arg_size));
          w->status = RUNNING;
          submitted++;
        }
      }
    }
    if (signal) {
      pthread_cond_broadcast(&(pool->cv));
    }
    /* If we're all done, we're not getting a wake-up call */
    if (done < arg_count) {
      pthread_cond_wait(&(pool->cv), &(pool->lock));
    }
  }
  pool->running = 0;
  pthread_cond_broadcast(&(pool->cv));
  pthread_mutex_unlock(&(pool->lock));
  return 1;
}

static void *_worker_func(void *the_worker) {
  struct _worker *worker = (struct _worker *) the_worker;
  pthread_mutex_t *lock = &(worker->parent->lock);
  pthread_cond_t *cv = &(worker->parent->cv);
  pthread_mutex_lock(lock);
  /* We have the lock so we're "initialized" - let the parent know */
  worker->parent->initializing--;
  pthread_cond_broadcast(cv);
  while (worker->status == RUNNABLE) {
    while (worker->status == RUNNABLE) {
      pthread_cond_wait(cv, lock);
    }
    if (worker->status == RUNNING) {
      pthread_mutex_unlock(lock);
      worker->retval = worker->func(worker->arg);
      pthread_mutex_lock(lock);
      worker->has_data = 1;
      worker->status = RUNNABLE;
    }
    pthread_cond_broadcast(cv);
  }
  pthread_mutex_unlock(lock);
  pthread_exit(NULL);
}
