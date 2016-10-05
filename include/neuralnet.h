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
#ifndef __HELIOS_NEURALNET__
#define __HELIOS_NEURALNET__

/**
 * An intial configuration for a neural net.
 */
typedef struct _netconfig {
  int layers; /* The number of layers in the network - including the input layer */
  const int *layer_sizes; /* The sizes of each layer in the network */
  double alpha; /* The learning rate for the network */
} netconfig;

/**
 * A neural network
 */
typedef struct _neuralnet neuralnet;

#endif /* __HELIOS_NEURALNET__ */
