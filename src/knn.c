/**
 * knn.c - K-Nearest Neighborhood
 */

#include "knn.h"

#include <limits.h>

#include "util.h"

/** The total number of classes */
#define NUM_CLASS 3

typedef struct dist_class {
  ull dist;
  int class;
} dist_class_t;

static void insert_neighbor(dist_class_t new_neighbor, dist_class_t* neighbors,
                            int len);
static int find_most_class(dist_class_t* neighbor, int len);

int find_nearest_neighbor_class(int* target, dataset_t* dataset, int k) {
  ull dist;

  // Stores the nearest k neighbor
  dist_class_t* neighbor = (dist_class_t*)malloc(sizeof(dist_class_t) * k);
  for (int i = 0; i < k; i++) {
    neighbor[i].dist = ULONG_MAX;
    neighbor[i].class = 0;
  }

  int cur_label, *cur_feature;
  for (int index = 0; index < dataset->len_recd; index++) {
    cur_label = dataset->label[index];
    cur_feature = dataset->feature[index];
    dist = dist_euclidean(target, cur_feature, dataset->len_feature);

    if (dist < neighbor[k - 1].dist) {
      insert_neighbor((dist_class_t){dist, cur_label}, neighbor, k);
    }
  }

  int max_class = find_most_class(neighbor, k);

  free(neighbor);

  return max_class;
}

static void insert_neighbor(dist_class_t new_neighbor, dist_class_t* neighbors,
                            int len) {
  int i;
  for (i = len - 1; i > 0; i--) {
    if (new_neighbor.dist < neighbors[i].dist) {
      neighbors[i].dist = neighbors[i - 1].dist;
      neighbors[i].class = neighbors[i - 1].class;
    } else {
      neighbors[i + 1].dist = new_neighbor.dist;
      neighbors[i + 1].class = new_neighbor.class;
      break;
    }
  }

  // Handle corner case
  if (i == 0 && new_neighbor.dist < neighbors[0].dist) {
    neighbors[0].dist = new_neighbor.dist;
    neighbors[0].class = new_neighbor.class;
  }
}

static int find_most_class(dist_class_t* neighbor, int len) {
  int* scoreboard = calloc(NUM_CLASS, sizeof(int));
  for (int i = 0; i < len; i++) {
    scoreboard[neighbor[i].class]++;
  }

  int max_class = 0;
  int max_num = 0;
  for (int i = 0; i < NUM_CLASS; i++) {
    if (max_num < scoreboard[i]) {
      max_class = i;
      max_num = scoreboard[i];
    }
  }
  free(scoreboard);

  return max_class;
}