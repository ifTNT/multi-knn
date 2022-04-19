/**
 * csv.c - Decoder implementation of Comma-Separated Values (CSV) Files
 * Reference: RFC 4180
 */

#include "csv.h"

#include <assert.h>
#include <string.h>

#include "list.h"

/** MAX_LINE_BUF - The size of line buffer to read file */
#define LEN_LINE_BUF 1024

/**
 * parse_col - Parse the columns from a line buffer
 * @cols: the pointer to columns. The space need to be assigned first
 * @buf: the line buffer containing the original string
 * @n: the maximum number of columns to be parse. 0 indicates MAX_FLD
 * @return: the numbers of parsed fields
 */
static size_t parse_col(char** cols, char* buf, size_t n);

/**
 * conv_recd - Convert columns to interger record
 * @recd: the pointer to the record to be modify
 * @cols: the parsed columns
 * @n: the lenght of columns
 */
static void conv_recd(recd_t* recd, char** cols, size_t n);

size_t csv_fromfile(csv_t* csv, FILE* fp) {
  size_t len_buf, len_cols;  // The length of line buffer and columns
  char* buf;                 // The buffer to store one line read from file
  char** cols;               // The buffer to store the parsed columns
  struct list_head* cur;     // The pointer to travel through link lists
  recd_t* new_recd;          // Point to new allocated records

  /* =================
   *   Parse Fields
   * ================= */
  /* Allocate buffer to parse fields */
  len_buf = LEN_LINE_BUF;
  buf = (char*)malloc(sizeof(char) * len_buf);
  if (getline(&buf, &len_buf, fp) == -1) {
    free(buf);
    return -1;
  }
  /* Parse the fields */
  csv->flds = (char**)malloc(sizeof(char*) * MAX_FLD);
  csv->len_fld = parse_col(csv->flds, buf, 0);

  /* =================
   *   Parse Records
   * ================= */
  /*
   * Since previous allocated buffer contains the content of fields.
   * We need to allocate new buffer to parse records.
   */
  len_buf = LEN_LINE_BUF;
  buf = (char*)malloc(sizeof(char) * len_buf);
  cols = (char**)malloc(sizeof(char*) * csv->len_fld);

  /* Parse the records */
  csv->len_recd = 0;
  INIT_LIST_HEAD(&csv->recds.list);
  cur = csv->recds.list.next;
  while (getline(&buf, &len_buf, fp) != -1) {
    len_cols = parse_col(cols, buf, csv->len_fld);
    assert(len_cols == csv->len_fld);

    // Allocate new space and convert value from column
    new_recd = (recd_t*)malloc(sizeof(recd_t));
    new_recd->val = malloc(sizeof(int) * len_cols);
    conv_recd(new_recd, cols, len_cols);

    // Append to the record list and increment the counter
    list_add(&new_recd->list, cur);
    cur = cur->next;
    csv->len_recd++;
  }
  /*
   * Release the buffer since the values are already stored into
   * recd_t structure
   */
  free(buf);
  free(cols);

  return csv->len_recd;
}

void csv_free(csv_t* csv) {
  /*
   * Release the space taken by records
   */
  struct list_head *head, *cur;
  recd_t* cur_recd;
  head = &csv->recds.list;
  cur = head->next;
  while (cur != head) {
    cur = cur->next;
    cur_recd = container_of(cur->prev, recd_t, list);
    free(cur_recd->val);
    free(cur_recd);
  }

  /*
   * Release the space taken by fields
   */
  if (csv->flds[0] != NULL) {
    free(csv->flds[0]);
  }
  free(csv->flds);
}

static size_t parse_col(char** cols, char* buf, size_t n) {
  const char* delm = ",\r\n";
  char* col;
  size_t cnt = 0;

  if (n == 0) n = MAX_FLD;

  // Tokenlize the line buffer
  col = strtok(buf, delm);
  while (col != NULL && cnt <= n) {
    *cols++ = col;
    cnt++;
    col = strtok(NULL, delm);
  }
  return cnt;
}

static void conv_recd(recd_t* recd, char** cols, size_t n) {
  for (int i = 0; i < n; i++) {
    recd->val[i] = atoi(cols[i]);
  }
}