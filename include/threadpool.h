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
#ifndef __HELIOS_THREAD_POOL__
#define __HELIOS_THREAD_POOL__
#include <stdio.h>

/**
 * A very simple blocking, mapping, thread pool. Takes in a mapper and a
 * set of jobs to submit to the mapper and waits until all are done.
 * You can get super far with this kind of simple parallelism.
 */
typedef struct _threadpool threadpool;

/**
 * Create a new thread pool.
 *
 * @param pool the return threadpool
 * @param threadcount the number of threads
 * @return 1 if successful.
 */
int threadpool_create(threadpool **pool, int threadcount);

/**
 * Destroy the thread pool given.
 *
 * @param pool the pool to destroy
 * @return whether it succeeded.
 */
int threadpool_destroy(threadpool *pool);

/**
 * Submit a set of jobs to this thread pool and block until they are done.
 *
 * @param pool the pool
 * @param retvals the return values of the jobs, or NULL if you don't care
 * @param mapper the function to run
 * @param arguments the arguments over which to run the function
 * @param arg_size the size of each argument
 * @param arg_count the number of arguments
 * @return 1 if successful.
 */
int threadpool_submit(threadpool *pool, void **retvals,
    void * (*mapper)(void *), unsigned char *arguments, size_t arg_size,
    int arg_count);

#endif /* __HELIOS_THREAD_POOL__ */
