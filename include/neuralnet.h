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

typedef double (*activation_func)(double);

typedef enum {
  ZERO_TO_ONE = 0,
  NEGATIVE_ONE_TO_ONE,
} activation_range;

/**
 * An intial configuration for a neural net.
 */
typedef struct _netconfig {
  int layers; /* The number of layers in the network (excluding input layer) */
  const int *layer_sizes; /* The sizes of each layer in the network */
  int dimensionality; /* The dimensionality of the input */
  activation_func activation; /* The activation function */
  activation_func activation_prime; /* Derivative of the activation function */
  activation_range range; /* The range for the activation function */
  int threads; /* How many threads to give to this net */
  double alpha; /* The learning rate for the network */
  double iscale; /* The input scale */
  int max_width; /* Upper bound on layer width (>= dimensionality too). */
} netconfig;

/**
 * A neural network
 */
typedef struct _neuralnet neuralnet;

/**
 * Create a new neural net matching the config spec given.
 * @param net pointer to the neural net to initialize
 * @param config the spec
 * @return did it succeed
 */
int neuralnet_create(neuralnet **net, netconfig config);

/**
 * Destroy the neural net given.
 * @param net the net
 * @return did it succeed
 */
int neuralnet_destroy(neuralnet *net);

/**
 * Train the neural network given with the inputs given.
 * This does one pass of feedforward and one pass of back propagation.
 * @param net the net
 * @param input the inputs
 * @param labels the correct labels for the inputs
 * @param input_count the number of inputs
 * @return did it succeed?
 */
int train(neuralnet *net, const double **inputs, const int *labels,
          int input_count);

/**
 * Classify the inputs given.
 * @param net the net
 * @param input the inputs
 * @param classes the classes the network came up with
 * @param input_count the number of inputs
 * @return did it succeed?
 */
int classify(neuralnet *net, const double **inputs, int *classes,
             int input_count);

#endif /* __HELIOS_NEURALNET__ */
