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
#include "neuralnet.h"

START_TEST(test_neuralnet_xor) {
  ck_assert_int_eq(2, 2);
}
END_TEST

Suite *neuralnet_suite(void) {
  Suite *s;
  s = suite_create("neuralnet");

  TCase *tc_xor = tcase_create("xor");
  tcase_add_test(tc_xor, test_neuralnet_xor);

  suite_add_tcase(s, tc_xor);

  return s;
}
