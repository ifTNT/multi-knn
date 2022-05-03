#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "csv.h"
#include "dataset.h"
#include "knn.h"
#include "list.h"
#include "util.h"

struct param {
  dataset_t* train_data;
  dataset_t* test_data;
  int* pred_label;
  ull job_begin;
  ull job_len;
  unsigned int k;
};

int read_dataset(dataset_t* dataset, char* file_name);
void* calc_knn(void* _param);
void print_accuracy(int* true_label, int* pred_label, int len);

int main(int argc, char* argv[]) {
  dataset_t train_data, test_data;
  unsigned int k, thread_num = 1;
  struct timespec start, finish;
  double elapsed;

  if (argc != 3 && argc != 2) {
    printf("Usage: %s K [#Threads]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  k = atoi(argv[1]);
  if (argc == 3) {
    thread_num = atoi(argv[2]);
  }
  printf("K: %d; #Threads: %d\n", k, thread_num);

  if (read_dataset(&train_data, "../data/diabetes_train.csv") != 0 ||
      read_dataset(&test_data, "../data/diabetes_test.csv") != 0) {
    exit(EXIT_FAILURE);
  }
  printf("Training dataset: %d records; %d features\n", train_data.len_recd,
         train_data.len_feature);
  printf("Testing dataset: %d records; %d features\n", test_data.len_recd,
         test_data.len_feature);
  printf("\n");

  // Begin wall-time measurement
  clock_gettime(CLOCK_MONOTONIC, &start);

  // Prepare shared spaces
  struct param* params =
      (struct param*)malloc(sizeof(struct param) * thread_num);
  pthread_t* thrds = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);
  int* pred_labels = (int*)malloc(sizeof(int) * test_data.len_recd);
  int err;

  // Launch threads
  for (int i = 0; i < thread_num; i++) {
    params[i].test_data = &test_data;
    params[i].train_data = &train_data;
    params[i].pred_label = pred_labels;
    params[i].k = k;
    params[i].job_len = test_data.len_recd / thread_num + 1;
    params[i].job_begin = params[i].job_len * i;
    err = pthread_create(&thrds[i], NULL, calc_knn, (void*)&params[i]);
    if (err) {
      pthread_perror("Error creating thread", err);
    }
  }

  // Wait threads completed
  for (int i = 0; i < thread_num; i++) {
    pthread_join(thrds[i], NULL);
  }

  // Measure elapsed time
  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1.0E9;
  printf("\nFinished. Elapsed: %f seconds\n", elapsed);
  print_accuracy(test_data.label, pred_labels, test_data.len_recd);

  // Cleanup
  free(params);
  free(thrds);
  free(pred_labels);
  dataset_free(&train_data);
  dataset_free(&test_data);

  return EXIT_SUCCESS;
}

int read_dataset(dataset_t* dataset, char* file_name) {
  FILE* fp = fopen(file_name, "r");
  csv_t csv;
  if (!fp) return -1;
  csv_fromfile(&csv, fp);
  dataset_fromcsv(dataset, &csv);

  csv_free(&csv);
  fclose(fp);
  return 0;
}

void* calc_knn(void* _param) {
  struct param* param = (struct param*)_param;
  ull i;
  for (int index = 0; index < param->job_len; index++) {
    i = index + param->job_begin;
    if (i >= param->test_data->len_recd) break;
    param->pred_label[i] = find_nearest_neighbor_class(
        param->test_data->feature[i], param->train_data, param->k);

    // Display progress
    if (index % 100 == 0) {
      fprintf(stderr, "\rProgress: %d %%", index * 100 / param->job_len + 1);
    }
  }

  pthread_exit(0);
}

void print_accuracy(int* true_label, int* pred_label, int len) {
  ull correct = 0, error = 0;
  for (int i = 0; i < len; i++) {
    if (true_label[i] == pred_label[i]) {
      correct++;
    } else {
      error++;
    }
  }
  printf("Correct: %d; Error: %d; Accuracy: %f\n", correct, error,
         (float)(correct) / (correct + error));

  return;
}