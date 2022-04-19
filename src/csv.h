#ifndef CSV_H
#define CSV_H

#include <stdio.h>

#include "list.h"

/**
 * csv.h - Decoder of Comma-Separated Values (CSV) Files
 * Reference: RFC 4180
 */

/** MAX_FLD - The maximum length of fields */
#define MAX_FLD 255

/** recd_t - CSV record object */
typedef struct {
  struct list_head list; /** List API */
  int* val;              /** Field of this record */
} recd_t;

/** csv_t - CSV object */
typedef struct {
  char** flds;     /** Name of fields */
  recd_t recds;    /** Linked list of records */
  size_t len_fld;  /** Total count of fields */
  size_t len_recd; /** Total count of records */
} csv_t;

/**
 * csv_fromfile - Read CSV from file
 * @csv: the pointer to structure of csv object
 * @fp: the file to read
 * @return: how many recored are read
 */
size_t csv_fromfile(csv_t* csv, FILE* fp);

/**
 * csv_free - Release the resource of a CSV object
 * @csv: the pointer to structure of csv object
 */
void csv_free(csv_t* csv);

#endif  // CSV_H