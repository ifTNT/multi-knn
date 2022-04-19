#include <stddef.h>

typedef unsigned long long int ull;

void pthread_perror(const char *prefix, int err);

ull dist(int *a, int *b, size_t n);