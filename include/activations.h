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
#ifndef __HELIOS_ACTIVATIONS__
#define __HELIOS_ACTIVATIONS__

/**
 * The sigmoid function.
 * @param x the input
 * @return sigmoid(x)
 */
double sigmoid(double x);

/**
 * The derivative of the sigmoid as a function of sigmoid(x)
 * @param sigmoid_x sigmoid(x)
 * @return the derivative of the sigmoid
 */
double sigmoid_prime(double sigmoid_x);

#endif /* __HELIOS_ACTIVATIONS__ */
