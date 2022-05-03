#include <stddef.h>

typedef unsigned long long int ull;

void pthread_perror(const char *prefix, int err);

ull dist_euclidean(const int *restrict a, const int *restrict b,
                   const size_t n);