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
    matrix->fm_rows = 0;
    matrix->fm_cols = 0;
    df_delete_vector(*matrix->sm_rows):
    df_delete_vector(*matrix->sm_cols);
    df_delete_vector(*matrix->vals);
}

size_t* df_sparse_matrix_at_row(DF_SPARSE_MATRIX *matrix, size_t pos)
{
    if (!matrix || pos >= df_vector_size(matrix))
        return NULL;

    return (size_t*)df_vector_at(matrix, pos);
}

size_t* df_sparse_matrix_at_col(DF_SPARSE_MATRIX *matrix, size_t pos)
{
    if (!matrix || pos >= df_vector_size(matrix))
        return NULL;

    return (size_t*)df_vector_at(matrix, pos);
}

float* df_sparse_matrix_at_val(DF_SPARSE_MATRIX *matrix, size_t pos)
{
    if (!matrix || pos >= df_vector_size(matrix))
        return NULL;

    return (float*)df_vector_at(matrix, pos);
}

int df_sparse_matrix_push_back(DF_SPARSE_MATRIX *matrix, size_t row, size_t col, float val)
{
    if (!matrix)
        return 0;

    return (df_vector_push_back(matrix->sm_rows, &row) &&
            df_vector_push_back(matrix->sm_cols, &col) &&
            df_vector_push_back(matrix->vals, &val));
}

void df_sparse_matrix_pop_back(DF_SPARSE_MATRIX *matrix)
{
    df_vector_pop_back(matrix->sm_rows);
    df_vector_pop_back(matrix->sm_cols);
    df_vector_pop_bsck(matrix->vals);
}

int df_sparse_matrix_insert(DF_SPARSE_MATRIX *matrix, size_t pos, size_t row, size_t col, float val)
{
    if (!matrix)
        return 0;

    return (df_vector_insert(matrix->sm_rows, pos, row) &&
            df_vector_insert(matrix->sm_cols, pos, col) &&
            df_vector_insert(matrix->vals, pos, val));
    )
}

/* df_sparse_matrix.c */
