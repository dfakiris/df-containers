#include "df_sparse_matrix.h"

struct DF_SPARSE_MATRIX
{
    DF_CVECTOR *sm_rows;
    DF_CVECTOR *sm_cols;
    DF_CVECTOR *vals;

    size_t fm_rows;
    size_t fm_cols;
};

/* df_sparse_matrix.c */
