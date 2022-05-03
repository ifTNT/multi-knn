#include "util.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

void pthread_perror(const char *prefix, int err) {
  fprintf(stderr, "%s: (%d)%s\n", prefix, err, strerror(err));
}

ull dist_euclidean(const int *restrict a, const int *restrict b,
                   const size_t n) {
  ull sum = 0;
  for (int i = 0; i < n; i++) {
    sum += (a[i] - b[i]) * (a[i] - b[i]);
  }
  return sum;
}