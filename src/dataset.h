#ifndef DATASET_H
#define DATASET_H

#include "csv.h"

/**
 * dataset.h - The structure of dataset which can be random access
 */

/** dataset_t - Dataset object */
typedef struct {
  int* label;         /** The label of the dataset */
  int** feature;      /** The features of the dataset */
  size_t len_feature; /** Total count of features */
  size_t len_recd;    /** Total count of records */
} dataset_t;

/**
 * dataset_fromcsv - Convert CSV to dataset
 * @dataset: the pointer to structure of dataset object
 * @csv: the pointer to structure of csv object
 */
void dataset_fromcsv(dataset_t* dataset, csv_t* csv);

/**
 * dataset_free - Release the resource of a dataset object
 * @dataset: the pointer to structure of dataset object
 */
void dataset_free(dataset_t* dataset);

#endif  // DATASET_H