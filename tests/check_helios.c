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

/* We're a little lazy, so we're just gonna straight up include our different
 * test suites. This isn't a problem - they're single purpose stuff */
#include <check.h>
#include <stdlib.h>
#include "check_threadpool.c"

int main(int argc, char **argv) {
  int number_failed;
  SRunner *sr;
  sr = srunner_create(threadpool_suite());
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
