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
#include <check.h>
#include <stdlib.h>
#include "threadpool.h"

/* How many elements to map over */
#define NUM_ELEMENTS 100

void mapper(void *arg, void *retval) {
  ck_assert_ptr_eq(retval, NULL);
  int *a = (int *) arg;
  *a *= 100;
}

void r_mapper(void *arg, void *retval) {
  ck_assert_ptr_ne(retval, NULL);
  int a = * ((int *) arg);
  *((int *) retval) = a * 100;
}

START_TEST(test_threadpool_noretval_basic) {
  threadpool *tp;
  int args[NUM_ELEMENTS];
  for (int i = 0; i < NUM_ELEMENTS; i++) {
    args[i] = i;
  }
  threadpool_create(&tp, 2);
  threadpool_submit(tp, NULL, mapper, (unsigned char *)args, sizeof(int),
                    NUM_ELEMENTS, 0);
  for (int i = 0; i < NUM_ELEMENTS; i++) {
    ck_assert_int_eq(args[i], i * 100);
  }
  threadpool_destroy(tp);
}
END_TEST

START_TEST(test_threadpool_retval_basic) {
  threadpool *tp;
  int args[NUM_ELEMENTS];
  int retvals[NUM_ELEMENTS];
  for (int i = 0; i < NUM_ELEMENTS; i++) {
    args[i] = i;
  }
  threadpool_create(&tp, 2);
  threadpool_submit(tp, (unsigned char *) retvals, r_mapper,
                    (unsigned char *)args, sizeof(int),
                    NUM_ELEMENTS, sizeof(int));

  for (int i = 0; i < NUM_ELEMENTS; i++) {
    ck_assert_int_eq(args[i], i);
    ck_assert_int_eq(retvals[i], i * 100);
  }
  threadpool_destroy(tp);
}
END_TEST

Suite *threadpool_suite(void) {
  Suite *s;
  s = suite_create("threadpool");

  TCase *tc_noretval = tcase_create("NoRetval");
  tcase_add_test(tc_noretval, test_threadpool_noretval_basic);

  TCase *tc_retval = tcase_create("Retval");
  tcase_add_test(tc_retval, test_threadpool_retval_basic);

  suite_add_tcase(s, tc_noretval);
  suite_add_tcase(s, tc_retval);
  return s;
}
