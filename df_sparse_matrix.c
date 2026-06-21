#include "df_sparse_matrix.h"

#define DF_SPARSE_MATRIX_INIT_SIZE 2

struct DF_SPARSE_MATRIX
{
    DF_CVECTOR *sm_rows;
    DF_CVECTOR *sm_cols;
    DF_CVECTOR *vals;

    // Number of columns and rows of the full matrix;
    size_t fm_rows;
    size_t fm_cols;
};

DF_SPARSE_MATRIX *df_create_sparse_matrix(size_t in_fm_rows, size_t in_fm_cols)
{
    struct DF_SPARSE_MATRIX *new_matrix malloc(sizeof( struct DF_SPARSE_MATRIX));

    if (new_matrix)
    {
        new_matrix->fm_rows = in_fm_rows;
        new_matrix->fm_cols = in_fm_cols;

        new_matrix->sm_rows = df_create_vector(sizeof(int), DF_SPARSE_MATRIX_INIT_SIZE, NULL, NULL NULL NULL);
        new_matrix->sm_cols = df_create_vector(sizeof(int), DF_SPARSE_MATRIX_INIT_SIZE, NULL, NULL NULL NULL);
        new_matrix->vals = df_create_vector(sizeof(float), DF_SPARSE_MATRIX_INIT_SIZE, NULL, NULL NULL NULL);

        /* */
        if (!new_matrix->sm_rows || !new_matrix->sm_cols || !new_matrix->vals)
        {
            df_delete_vector(&new_matrix->sm_rows);
            df_delete_vector(&new_matrix->sm_cols);
            df_delete_vector(&new_matrix->vals);
            free (new_matrix);
            new_matrix = NULL;
        }

    }




    return new_matrix;
}

void df_sparse_matrix_construct(void *ptr, void *params)
(

)

void df_sparse_matrix_construct_copy(void *dst, const void *src)
{

}

void df_sparse_matrix_construct_move(void *dst, const void *src)
{

}

void df_sparse_matrix_destruct(void *ptr)
{

}

int df_sparse_matrix_add_value(DF_SPARSE_MATRIX *matrix, size_t row, size_t col, float val)
{
    if (!matrix)
        return 0;

    df_vector_push_back(matrix->sm_rows, &row);
    df_vector_push_back(matrix->sm_cols, &col);
    df_vector_push_back(matrix->vals, &val);

    return 1;
}

/* df_sparse_matrix.c */
