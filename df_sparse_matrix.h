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

size_t* df_sparse_matrix_at_row(DF_SPARSE_MATRIX *matrix, size_t pos);

size_t* df_sparse_matrix_at_col(DF_SPARSE_MATRIX *matrix, size_t pos);

float* df_sparse_matrix_at_val(DF_SPARSE_MATRIX* matrix, size_t pos);

int df_sparse_matrix_push_back(size_t row, size_t col, float val);

void df_sparse_matrix_pop_back(DF_SPARSE_MATRIX *matrix);

int df_sparse_matrix_insert(DF_SPARSE_MATRIX *matrix, size_t pos, size_t row, size_t col, float val)

#endif /* DF_SPARSE_MATRIX */
