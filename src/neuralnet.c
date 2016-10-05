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

struct _neuralnet {
  netconfig config; /* The net's configuration */
  threadpool *pool; /* Our threadpool */
  double *w; /* All the weights. See below for indexing */
  double *oldw; /* The old unadjusted weights (for backpropagation) */
  double *derr; /* All the error derivatives. */
  double *out; /* All the neuron outputs. */
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
   * two sets of old weights: the ones for the previous layer, to adjust
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
