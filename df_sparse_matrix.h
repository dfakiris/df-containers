#ifndef DF_SPARSE_MATRIX
#define DF_SPARSE_MATRIX

#include <stddef.h> // NULL

#include "df_vector.h"

typedef struct DF_DF_SPARSE_MATRIX DF_SPARSE_MATRIX;

DF_SPARSE_MATRIX *df_create_sparse_matrix(size_t in_fm_rows, size_t in_fm_cols);

void df_sparse_matrix_construct(void *ptr, void *params);

void df_sparse_matrix_construct_copy(void *dst, const void *src);

void df_sparse_matrix_construct_move(void *dst, const void *src);

void df_sparse_matrix_destruct(void *ptr);

int df_sparse_matrix_add_value(size_t row, size_t col, float val);

#endif /* DF_SPARSE_MATRIX */
