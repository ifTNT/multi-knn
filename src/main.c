#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "csv.h"
#include "dataset.h"
#include "list.h"
#include "util.h"

struct dist_class {
  ull dist;
  int class;
};

int find_nearest_neighbor_class(int* target, dataset_t* dataset, int k) {
  ull dist;

  // Neighbor stores the nearest k neighbor
  struct dist_class* neighbor =
      (struct dist_class*)malloc(sizeof(struct dist_class) * k);
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
      // Insert new neighbor
      int i;
      for (i = k - 1; i > 0; i--) {
        if (dist < neighbor[i].dist) {
          neighbor[i].dist = neighbor[i - 1].dist;
          neighbor[i].class = neighbor[i - 1].class;
        } else {
          neighbor[i + 1].dist = dist;
          neighbor[i + 1].class = cur_label;
          break;
        }
      }
      if (i == 0 && dist < neighbor[0].dist) {
        neighbor[0].dist = dist;
        neighbor[0].class = cur_label;
      }
    }
  }

  // Find most class
  int classes = 3;
  int* scoreboard = calloc(classes, sizeof(int));
  for (int i = 0; i < k; i++) {
    scoreboard[neighbor[i].class]++;
  }

  int max_class = 0;
  int max_num = 0;
  for (int i = 0; i < classes; i++) {
    if (max_num < scoreboard[i]) {
      max_class = i;
      max_num = scoreboard[i];
    }
  }
  free(neighbor);
  free(scoreboard);
  return max_class;
}

int main(int argc, char* argv[]) {
  FILE *train_fp, *test_fp;
  csv_t train_csv, test_csv;
  unsigned int k, thread_num = 1;

  if (argc != 3 && argc != 2) {
    printf("usage: %s K [#Threads]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  k = atoi(argv[1]);
  if (argc == 3) {
    thread_num = atoi(argv[2]);
  }
  printf("K=%d, #Threads=%d\n", k, thread_num);

  train_fp = fopen("../data/diabetes_train.csv", "r");
  test_fp = fopen("../data/diabetes_test.csv", "r");
  if (!train_fp || !test_fp) exit(EXIT_FAILURE);
  csv_fromfile(&train_csv, train_fp);
  csv_fromfile(&test_csv, test_fp);
  printf("Training dataset: %d records; %d fields\n", train_csv.len_recd,
         train_csv.len_fld);
  printf("Testing dataset: %d records; %d fields\n", test_csv.len_recd,
         test_csv.len_fld);

  dataset_t train_data, test_data;

  dataset_fromcsv(&train_data, &train_csv);
  dataset_fromcsv(&test_data, &test_csv);

  int pred_label, true_label;
  ull correct = 0, error = 0;
  for (int i = 0; i < test_data.len_recd; i++) {
    true_label = test_data.label[i];
    pred_label =
        find_nearest_neighbor_class(test_data.feature[i], &train_data, k);
    if (true_label == pred_label) {
      correct++;
    } else {
      error++;
    }
    if ((correct + error) % 100 == 0) {
      fprintf(stderr, "correct: %d; error: %d; acc: %f\n", correct, error,
              (float)(correct) / (correct + error));
    }
  }

  fprintf(stderr, "correct: %d; error: %d; acc: %f\n", correct, error,
          (float)(correct) / (correct + error));

  dataset_free(&train_data);
  dataset_free(&test_data);
  csv_free(&train_csv);
  csv_free(&test_csv);

  return EXIT_SUCCESS;
}