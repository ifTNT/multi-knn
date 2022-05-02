#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "csv.h"
#include "dataset.h"
#include "knn.h"
#include "list.h"
#include "util.h"

int main(int argc, char* argv[]) {
  FILE *train_fp, *test_fp;
  csv_t train_csv, test_csv;
  unsigned int k, thread_num = 1;

  if (argc != 3 && argc != 2) {
    printf("Usage: %s K [#Threads]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  k = atoi(argv[1]);
  if (argc == 3) {
    thread_num = atoi(argv[2]);
  }
  printf("K: %d; #Threads: %d\n", k, thread_num);

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

  struct timespec start, finish;
  double elapsed;

  clock_gettime(CLOCK_MONOTONIC, &start);

  printf("\n");

  int* pred_labels = (int*)malloc(sizeof(int) * test_data.len_recd);
  for (int i = 0; i < test_data.len_recd; i++) {
    pred_labels[i] =
        find_nearest_neighbor_class(test_data.feature[i], &train_data, k);
    if (i % 100 == 0) {
      fprintf(stderr, "\rProgress: %d %%", i * 100 / test_data.len_recd + 1);
    }
  }

  printf("\n");

  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1.0E9;
  printf("Finished. Elapsed: %f seconds\n", elapsed);

  // Calculate the accuracy
  int true_label, pred_label;
  ull correct = 0, error = 0;
  for (int i = 0; i < test_data.len_recd; i++) {
    true_label = test_data.label[i];
    pred_label = pred_labels[i];
    if (true_label == pred_label) {
      correct++;
    } else {
      error++;
    }
  }

  printf("Correct: %d; Error: %d; Accuracy: %f\n", correct, error,
         (float)(correct) / (correct + error));

  dataset_free(&train_data);
  dataset_free(&test_data);
  csv_free(&train_csv);
  csv_free(&test_csv);

  return EXIT_SUCCESS;
}