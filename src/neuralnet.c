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
#include "threadpool.h"
#include "neuralnet.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * Get the weight by indexing into the weight table given.
 * @param warray the weight array
 * @param max_width the maximum width of any given layer
 * @param layer the layer we want
 * @param neuron the neuron we want
 * @param weight the weight we want for the neuron
 */
#define GET_WEIGHT(warray, max_width, layer, neuron, weight) \
  warray[max_width * max_width * layer + max_width * neuron + weight]

/**
 * Do a feed forward pass on the network
 */
static void _feed_forward(neuralnet *net, const double **inputs,
                          int input_count);

/**
 * Do a backpropagate on the network, adjusting stuff as we go.
 */
static void _back_propagate(neuralnet *net, const double **inputs,
                            const int *labels, int input_count);

/**
 * The worker for the feedforward pass.
 */
static void _ff_worker(void *in, void *out);

/**
 * The worker for the first backpropagate iteration, that deals with the
 * output layer
 */
static void _output_bp_worker(void *in, void *out);

/**
 * The worker for the back propagate pass.
 */
static void _bp_worker(void *in, void *out);

struct _neuralnet {
  netconfig config; /* The net's configuration */
  threadpool *pool; /* Our threadpool */
  double *w; /* All the weights. See below for indexing */
  double *oldw; /* The old unadjusted weights (for backpropagation) */
  double *derr; /* All the error derivatives. */
  double *out; /* All the neuron outputs. */
};

/**
 * A structure containing parameters for each worker for each layer.
 */
struct _layer_params {
  const netconfig *config; /* The network configuration */
  int start; /* The first neuron to look at, inclusive */
  int end; /* The last neuron to look at, exclusive */
  double *weights; /* The weights for this layer */
  double *oldw_r; /* The old weights of the next layer */
  double *oldw_w; /* The old weights of this layer (for writing) */
  int w_count; /* How many weights are there in this layer, per neuron */
  int wnext_count; /* How many weights in the next layer connect to each neuron
                    * in this one. */
  double *inputs; /* The inputs to this layer */
  double *derr_r; /* The derivative of the error at the next layer */
  double *derr_w; /* The derivative of the error at this layer */
  double *outputs; /* The outputs for this layer */
  int *targets; /* The targets - only if this corresponds to the last layer */
  double ifactor; /* The input factor for this layer */
};

int neuralnet_create(neuralnet **retval, netconfig config) {
  neuralnet *net = malloc(sizeof(struct _neuralnet));
  if (!net) {
    perror("neuralnet_create");
    return 0;
  }
  net->config = config;
  if (!threadpool_create(&(net->pool), net->config.threads)) {
    perror("neuralnet_create");
    free(net);
    return 0;
  }
  size_t sz = net->config.max_width * net->config.max_width *
              net->config.layers;
  net->w = malloc(sizeof(double) * sz);
  if (!net->w) {
    perror("neuralnet_create");
    threadpool_destroy(net->pool);
    free(net);
    return 0;
  }
  net->derr = malloc(sizeof(double) * net->config.max_width *
                     net->config.layers);
  if (!net->derr) {
    perror("neuralnet_create");
    threadpool_destroy(net->pool);
    free(net->w);
    free(net);
    return 0;
  }
  net->out = malloc(sizeof(double) * net->config.max_width *
                    net->config.layers);
  if (!net->out) {
    perror("neuralnet_create");
    threadpool_destroy(net->pool);
    free(net->derr);
    free(net->w);
    free(net);
    return 0;
  }
  /* That 2 is to save a bit of memory; after all we only ever need
   * two sets of old weights: the ones for the next layer, to adjust
   * this layer, and an empty array for the ones for this layer so that
   * we can save them as we adjust the layer.
   * See the backpropagation method for how this works in practice.
   * This also prevents an expensive memcpy before every layer's adjustment.
   */
  net->oldw = malloc(sizeof(double) * net->config.max_width *
                     net->config.max_width * 2);
  if (!net->oldw) {
    perror("neuralnet_create");
    threadpool_destroy(net->pool);
    free(net->out);
    free(net->derr);
    free(net->w);
    free(net);
  }
  *retval = net;
  return 1;
}

int train(neuralnet *net, const double **inputs, const int *labels,
          int input_count) {
  _feed_forward(net, inputs, input_count);
  _back_propagate(net, inputs, labels, input_count);
  return 1;
}

int classify(neuralnet *net, const double **inputs, double *results,
             int input_count) {
  _feed_forward(net, inputs, input_count);
  /* TODO Cook up how the results come out of here */
  return 1;
}

int neuralnet_destroy(neuralnet *net) {
  int rc = 1;
  /* I mean it's not like we can do anything if we fail to destroy something
   * and we do still have to delete everything else.
   * So just store the rc and hope the caller knows what to do */
  rc &= threadpool_destroy(net->pool);
  free(net->out);
  free(net->derr);
  free(net->w);
  free(net->oldw);
  free(net);
  return rc;
}

static void _feed_forward(neuralnet *net, const double **inputs,
                          int input_count) {

}

static void _back_propagate(neuralnet *net, const double **inputs,
                            const int *labels, int input_count) {

}

static void _ff_worker(void *in, void *out) {
  struct _layer_params *params = (struct _layer_params *) in;
  for (int neuron = params->start; neuron < params->end; neuron++) {
    params->outputs[neuron] = 0;
    for (int input = 0; input < params->w_count; input++) {
      /* It's the zeroth layer because params->weight already points at the
       * weigths for this layer (not for the entire network) */
      params->outputs[neuron] += GET_WEIGHT(params->weights,
          params->config->max_width, 0, neuron, input) * params->inputs[input];
    }
    params->outputs[neuron] = params->config->activation(params->ifactor *
        params->outputs[neuron]);
  }
}

static void _output_bp_worker(void *in, void *out) {
  struct _layer_params *params = (struct _layer_params *) in;
  int mw = params->config->max_width;
  for (int neuron = params->start; neuron < params->end; neuron++) {
    double out = params->outputs[neuron];
    double error = params->targets[neuron] - out;
    /* I double dog dare you to differentiate the error */
    double derr = error * params->config->activation_prime(out);
    params->derr_w[neuron] = derr;
    for (int input = 0; input < params->w_count; input++) {
      GET_WEIGHT(params->oldw_w, mw, 0, neuron, input) =
        GET_WEIGHT(params->weights, mw, 0, neuron, input);
      GET_WEIGHT(params->weights, mw, 0, neuron, input) +=
        params->config->alpha * derr * params->inputs[input];
    }
  }
}

static void _bp_worker(void *in, void *out) {
  struct _layer_params *params = (struct _layer_params *) in;
  int mw = params->config->max_width;
  /* This has to be three separate loops to prevent the mortal sin of
   * iterating column-wise over an array. */
  /* Start by setting the derivatives to 0; this is so we can reuse the array
   * without having to allocate a new one in here */
  for (int neuron = params->start; neuron < params->end; neuron++) {
    params->derr_w[neuron] = 0;
  }
  /* Now calculate ERRORS (not dErrors) for each neuron */
  for (int next = 0; next < params->wnext_count; next++) {
    for (int neuron = params->start; neuron < params->end; neuron++) {
      params->derr_w[neuron] += params->derr_r[next] *
        GET_WEIGHT(params->oldw_r, mw, 0, next, neuron);
    }
  }
  /* That was the worst of it. Now just adjust the weights as normal */
  for (int neuron = params->start; neuron < params->end; neuron++) {
    double out = params->outputs[neuron];
    params->derr_w[neuron] *= params->config->activation_prime(out);
    for (int input = 0; input < params->w_count; input++) {
      GET_WEIGHT(params->oldw_w, mw, 0, neuron, input) =
        GET_WEIGHT(params->weights, mw, 0, neuron, input);
      GET_WEIGHT(params->weights, mw, 0, neuron, input) =
        params->config->alpha * params->derr_w[neuron] * params->inputs[input];
    }
  }
}
