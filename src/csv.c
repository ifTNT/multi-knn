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
 * parse_col - Parse the columns from file
 * @cols: the pointer to columns. The space need to be assigned first
 * @fp: the file descriptor to parse
 * @n: the maximum number of columns to be parse. 0 indicates MAX_FLD
 * @buf: the buffer to be allocate to read file
 * @return: the numbers of parsed fields
 */
static size_t parse_col(char** cols, FILE* fp, size_t n, char** buf);

/**
 * conv_recd - Convert columns to interger record
 * @recd: the pointer to the record to be modify
 * @cols: the parsed columns
 * @n: the lenght of columns
 */
static void conv_recd(recd_t* recd, char** cols, size_t n);

size_t csv_fromfile(csv_t* csv, FILE* fp) {
  /* The buffer to store one line read from file */
  char* buf;
  /* The buffer to store the parsed columns */
  char** cols = (char**)malloc(sizeof(char*) * MAX_FLD);
  /* The length of parsed columns */
  size_t len_cols = 0;

  /** The pointer to travel through link lists */
  struct list_head* cur;
  recd_t* new_recd;

  /* Allocate the structure */
  csv->flds = (char**)malloc(sizeof(char*) * MAX_FLD);
  INIT_LIST_HEAD(&csv->recds.list);
  csv->len_fld = 0;
  csv->len_recd = 0;

  /* Parse the fields */
  if ((csv->len_fld = parse_col(csv->flds, fp, 0, &buf)) == -1) {
    return -1;
  }

  /* Parse the records */
  cur = csv->recds.list.next;
  while ((len_cols = parse_col(cols, fp, csv->len_fld, &buf)) != -1) {
    assert(len_cols == csv->len_fld);

    // Allocate new space and convert value from column
    new_recd = (recd_t*)malloc(sizeof(recd_t));
    new_recd->val = malloc(sizeof(int) * len_cols);
    conv_recd(new_recd, cols, len_cols);

    // Append to the record list and increment the counter
    list_add(&new_recd->list, cur);
    cur = cur->next;
    csv->len_recd++;

    // Release the line buffer since the values are already stored into
    // recd_t structure
    free(buf);
  }
}

void csv_free(csv_t* csv) {
  struct list_head* cur = csv->recds.list.next;
  while (cur != &csv->recds.list) {
    __list_del_entry(cur);
    free(cur);
    cur = cur->next;
  }
  free(csv->flds);
}

static size_t parse_col(char** cols, FILE* fp, size_t n, char** buf) {
  const char* delm = ",\r\n";
  char *_buf = NULL, *col;
  size_t cnt = 0, len;

  if (n == 0) n = MAX_FLD;

  // Let getline() to implicit allocate a new buffer
  if (getline(&_buf, &len, fp) == -1) {
    return -1;
  }
  *buf = _buf;

  // Tokenlize the string that just read
  col = strtok(_buf, delm);
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