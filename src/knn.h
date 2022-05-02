#ifndef KNN_H
#define KNN_H

/**
 * knn.h - K-Nearest Neighborhood
 */

#include "dataset.h"

/**
 * find_nearest_neighbor_class - Find the class of nearest neighbor
 * @target: the target feature to be find
 * @dataset: the searching space
 * @k: how many neighbor should be considered
 * @return: the class of nearest neighbor
 */
int find_nearest_neighbor_class(int* target, dataset_t* dataset, int k);

#endif  // KNN_H