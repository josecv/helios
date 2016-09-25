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

START_TEST(test_threadpool_noretval) {

}
END_TEST

Suite *threadpool_suite(void) {
  Suite *s;
  TCase *tc_core;
  s = suite_create("threadpool");
  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_threadpool_noretval);
  suite_add_tcase(s, tc_core);
  return s;
}
