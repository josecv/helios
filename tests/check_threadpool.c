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

void *mapper(void *arg) {
  int *a = (int *) arg;
  *a *= 100;
  return NULL;
}

START_TEST(test_threadpool_noretval_basic) {
  threadpool *tp;
  int args[NUM_ELEMENTS];
  for (int i = 0; i < NUM_ELEMENTS; i++) {
    args[i] = i;
  }
  threadpool_create(&tp, 2);
  threadpool_submit(tp, NULL, mapper, (unsigned char *)args, sizeof(int),
                    NUM_ELEMENTS);
  for (int i = 0; i < NUM_ELEMENTS; i++) {
    ck_assert_int_eq(args[i], i * 100);
  }
  threadpool_destroy(tp);
}
END_TEST

Suite *threadpool_suite(void) {
  Suite *s;
  TCase *tc_noretval;
  s = suite_create("threadpool");
  tc_noretval = tcase_create("NoRetval");
  tcase_add_test(tc_noretval, test_threadpool_noretval_basic);
  suite_add_tcase(s, tc_noretval);
  return s;
}
