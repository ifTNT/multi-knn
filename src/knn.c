/**
 * knn.c - K-Nearest Neighborhood
 */

#include "knn.h"

#include <limits.h>

#include "util.h"

/** The total number of classes */
#define NUM_CLASS 3

#define MAX_K 1024

typedef struct dist_class {
  ull dist;
  int class;
} dist_class_t;

static ull dist_euclidean(const int* restrict a, const int* restrict b,
                          const size_t n);
static void insert_neighbor(dist_class_t new_neighbor, dist_class_t* neighbors,
                            int len);
static int find_most_class(dist_class_t* neighbor, int len);

int find_nearest_neighbor_class(int* target, dataset_t* dataset, int k) {
  ull dist;

  // Stores the nearest k neighbor
  dist_class_t neighbor[MAX_K];
  for (int i = 0; i < k; i++) {
    neighbor[i].dist = ULONG_MAX;
    neighbor[i].class = -1;
  }

  int cur_label, *cur_feature;
  for (int i = 0; i < dataset->len_recd; i++) {
    cur_label = dataset->label[i];
    cur_feature = dataset->feature[i];
    dist = dist_euclidean(target, cur_feature, dataset->len_feature);

    if (dist < neighbor[0].dist) {
      insert_neighbor((dist_class_t){dist, cur_label}, neighbor, k);
    }
  }

  int max_class = find_most_class(neighbor, k);

  return max_class;
}

#define VECTOR_LEN 4
typedef int v4si __attribute__((vector_size(VECTOR_LEN * sizeof(int))));
static inline ull dist_euclidean(const int* restrict a, const int* restrict b,
                                 const size_t n) {
  v4si va, vb, vs, vss = {0};
  ull sum, tmp;
  size_t vec_bound = n - n % VECTOR_LEN;

  // Use vector arithmetic when possible
  for (int i = 0; i < vec_bound; i += VECTOR_LEN) {
    va = *(v4si*)(a + i);
    vb = *(v4si*)(b + i);
    vs = va - vb;
    vss += vs * vs;
  }
  sum = vss[0] + vss[1] + vss[2] + vss[3];

  // Fallback to scaler for remaining part
  for (int i = vec_bound; i < n; i++) {
    tmp = a[i] - b[i];
    sum += tmp * tmp;
  }
  return sum;
}

static inline void insert_neighbor(dist_class_t new_neighbor,
                                   dist_class_t* neighbors, int len) {
  int i;
  for (i = 0; new_neighbor.dist < neighbors[i].dist && i < len; i++) {
    neighbors[i] = neighbors[i + 1];
  }
  if (i != 0) neighbors[i - 1] = new_neighbor;
}

static inline int find_most_class(dist_class_t* neighbor, int len) {
  int scoreboard[NUM_CLASS] = {0};
  for (int i = 0; i < len; i++) {
    if (neighbor[i].class == -1) continue;
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

  return max_class;
}