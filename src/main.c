#include <stdio.h>
#include <stdlib.h>

#include "csv.h"
#include "list.h"

int main(int argc, char* argv[]) {
  FILE* fp = fopen("../data/diabetes_train.csv", "r");
  if (fp == NULL) exit(EXIT_FAILURE);

  csv_t my_csv;
  csv_fromfile(&my_csv, fp);

  /* Print the fields */
  for (int i = 0; i < my_csv.len_fld; i++) {
    printf("%s\t", my_csv.flds[i]);
  }
  printf("\n");

  /* Print the records */
  struct list_head* head = &my_csv.recds.list;
  struct list_head* cur = head->next;
  recd_t* cur_recd;
  while (cur != head) {
    cur_recd = container_of(cur, recd_t, list);
    for (int i = 0; i < my_csv.len_fld; i++) {
      // printf("%d\t", cur_recd->val[i]);
    }
    // printf("\n");
    cur = cur->next;
  }
  printf("Total %d records; %d fields\n", my_csv.len_recd, my_csv.len_fld);

  return EXIT_SUCCESS;
}