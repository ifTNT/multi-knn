#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "csv.h"
#include "list.h"
#include "util.h"

struct dist_class {
  ull dist;
  int class;
};

int find_nearest_neighbor_class(int* target, csv_t* dataset, int k) {
  struct list_head* head = &dataset->recds.list;
  struct list_head* cur = head->next;
  recd_t* cur_recd;
  ull dist;

  // Neighbor stores the nearest k neighbor
  struct dist_class* neighbor =
      (struct dist_class*)malloc(sizeof(struct dist_class) * k);
  for (int i = 0; i < k; i++) {
    neighbor[i].dist = ULONG_MAX;
    neighbor[i].class = 0;
  }

  while (cur != head) {
    cur_recd = container_of(cur, recd_t, list);

    dist = dist_euclidean(target + 1, cur_recd->val + 1, dataset->len_fld - 1);

    if (dist < neighbor[k - 1].dist) {
      // Insert new neighbor
      int i;
      for (i = k - 1; i > 0; i--) {
        if (dist < neighbor[i].dist) {
          neighbor[i].dist = neighbor[i - 1].dist;
          neighbor[i].class = neighbor[i - 1].class;
        } else {
          neighbor[i + 1].dist = dist;
          neighbor[i + 1].class = cur_recd->val[0];
          break;
        }
      }
      if (i == 0 && dist < neighbor[0].dist) {
        neighbor[0].dist = dist;
        neighbor[0].class = cur_recd->val[0];
      }
    }
    cur = cur->next;
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

  struct list_head* head = &test_csv.recds.list;
  struct list_head* cur = head->next;
  recd_t* cur_recd;
  int pred_label, true_label;
  ull correct = 0, error = 0;
  while (cur != head) {
    cur_recd = container_of(cur, recd_t, list);
    true_label = cur_recd->val[0];
    pred_label = find_nearest_neighbor_class(cur_recd->val, &train_csv, k);
    if (true_label == pred_label) {
      correct++;
    } else {
      error++;
    }
    if ((correct + error) % 100 == 0) {
      fprintf(stderr, "correct: %d; error: %d; acc: %f\n", correct, error,
              (float)(correct) / (correct + error));
    }
    cur = cur->next;
  }

  fprintf(stderr, "correct: %d; error: %d; acc: %f\n", correct, error,
          (float)(correct) / (correct + error));

  csv_free(&train_csv);
  csv_free(&test_csv);

  return EXIT_SUCCESS;
}