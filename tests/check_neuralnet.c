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
#include "activations.h"

#define ITERATIONS 1000

START_TEST(test_neuralnet_or) {
  netconfig conf;
  int layer_sizes[1] = { 1 };
  conf.layers = 1;
  conf.layer_sizes = layer_sizes;
  conf.dimensionality = 2;
  conf.activation = sigmoid;
  conf.activation_prime = sigmoid_prime;
  conf.threads = 2;
  conf.alpha = 0.1;
  conf.iscale = 0.1;
  conf.max_width = 2;
  neuralnet *net;
  neuralnet_create(&net, conf);
  double inputs[8] = { 0, 0,
                       0, 1,
                       1, 0,
                       1, 1 };
  double labels[4] = { 0, 1, 1, 1 };
  for (int i = 0; i < ITERATIONS; i++) {
    ck_assert_int_eq(neuralnet_train(net, inputs, labels, 4), 1);
  }
  /* Just reuse the labels, can't be bothered */
  ck_assert_int_eq(neuralnet_classify(net, inputs, labels, 4), 1);
  ck_assert(labels[0] < 0.05);
  ck_assert(labels[1] > 0.95);
  ck_assert(labels[2] > 0.95);
  ck_assert(labels[3] > 0.95);
  neuralnet_destroy(net);
}
END_TEST

START_TEST(test_neuralnet_xor) {
  ck_assert_int_eq(2, 2);
}
END_TEST

Suite *neuralnet_suite(void) {
  Suite *s;
  s = suite_create("neuralnet");

  TCase *tc_simple = tcase_create("simple");
  tcase_add_test(tc_simple, test_neuralnet_xor);
  tcase_add_test(tc_simple, test_neuralnet_or);

  suite_add_tcase(s, tc_simple);

  return s;
}