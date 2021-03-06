/**
 * dataset.c - The structure of dataset which can be random access
 */

#include "dataset.h"

#include <string.h>

void dataset_fromcsv(dataset_t* dataset, csv_t* csv) {
  struct list_head* head = &csv->recds.list;
  struct list_head* cur = head->next;
  recd_t* cur_recd;
  int cur_index = 0;

  dataset->label = (int*)malloc(sizeof(int) * csv->len_recd);
  dataset->feature = (int**)malloc(sizeof(int*) * csv->len_recd);
  dataset->len_recd = csv->len_recd;
  dataset->len_feature = csv->len_fld - 1;

  while (cur != head) {
    cur_recd = container_of(cur, recd_t, list);
    dataset->label[cur_index] = cur_recd->val[0];
    dataset->feature[cur_index] =
        (int*)malloc(sizeof(int) * dataset->len_feature);
    memcpy(dataset->feature[cur_index], cur_recd->val + 1,
           sizeof(int) * dataset->len_feature);

    cur = cur->next;
    cur_index++;
  }
}

void dataset_free(dataset_t* dataset) {
  for (int i = 0; i < dataset->len_recd; i++) {
    free(dataset->feature[i]);
  }
  free(dataset->label);
  free(dataset->feature);
}