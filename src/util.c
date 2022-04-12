#include <errno.h>
#include <stdio.h>
#include <string.h>

void pthread_perror(const char *prefix, int err) {
  fprintf(stderr, "%s: (%d)%s\n", prefix, err, strerror(err));
}
