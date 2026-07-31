#include "matrix.h"
#include "config.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static void fatal_memory_error(const char *context)
{
    fprintf(stderr, "Kritik bellek hatasi: %s icin bellek ayrilamadi.\n", context);
    exit(EXIT_FAILURE);
}

Matrix matrix_create(size_t rows, size_t cols)
{
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;
    matrix.data = NULL;

    if (rows == 0 || cols == 0) {
        return matrix;
    }

    /* Integer tasmasini kontrol et: rows * cols carpimi ve bunun
     * sizeof(double) ile carpimi taskan olmamali. */
    if (rows > (SIZE_MAX / cols)) {
        fprintf(stderr, "Kritik hata: matris boyutu (%zux%zu) tasmaya neden oluyor.\n", rows, cols);
        exit(EXIT_FAILURE);
    }

    size_t element_count = rows * cols;

    if (element_count > (SIZE_MAX / sizeof(double))) {
        fprintf(stderr, "Kritik hata: matris boyutu (%zux%zu) bellek tasmasina neden oluyor.\n", rows, cols);
        exit(EXIT_FAILURE);
    }

    matrix.data = (double *)calloc(element_count, sizeof(double));
    if (matrix.data == NULL) {
        fatal_memory_error("matris verisi");
    }

    return matrix;
}

void matrix_destroy(Matrix *matrix)
{
    if (matrix == NULL) {
        return;
    }

    free(matrix->data);
    matrix->data = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

bool matrix_copy(Matrix *destination, const Matrix *source)
{
    if (destination == NULL || source == NULL) {
        return false;
    }

    Matrix new_matrix = matrix_create(source->rows, source->cols);

    if (source->data != NULL && new_matrix.data != NULL) {
        size_t element_count = source->rows * source->cols;
        for (size_t i = 0; i < element_count; i++) {
            new_matrix.data[i] = source->data[i];
        }
    }

    *destination = new_matrix;
    return true;
}

double matrix_get(const Matrix *matrix, size_t row, size_t col)
{
    assert(matrix != NULL);
    assert(matrix->data != NULL);
    assert(row < matrix->rows);
    assert(col < matrix->cols);

    return matrix->data[row * matrix->cols + col];
}

void matrix_set(Matrix *matrix, size_t row, size_t col, double value)
{
    assert(matrix != NULL);
    assert(matrix->data != NULL);
    assert(row < matrix->rows);
    assert(col < matrix->cols);

    matrix->data[row * matrix->cols + col] = value;
}

void matrix_swap_rows(Matrix *matrix, size_t row1, size_t row2)
{
    assert(matrix != NULL);
    assert(row1 < matrix->rows);
    assert(row2 < matrix->rows);

    if (row1 == row2) {
        return;
    }

    for (size_t col = 0; col < matrix->cols; col++) {
        double temp = matrix_get(matrix, row1, col);
        matrix_set(matrix, row1, col, matrix_get(matrix, row2, col));
        matrix_set(matrix, row2, col, temp);
    }
}

void matrix_scale_row(Matrix *matrix, size_t row, double scalar)
{
    assert(matrix != NULL);
    assert(row < matrix->rows);

    for (size_t col = 0; col < matrix->cols; col++) {
        double value = matrix_get(matrix, row, col) * scalar;
        matrix_set(matrix, row, col, value);
    }
}

void matrix_add_scaled_row(
    Matrix *matrix,
    size_t target_row,
    size_t source_row,
    double scalar
)
{
    assert(matrix != NULL);
    assert(target_row < matrix->rows);
    assert(source_row < matrix->rows);

    for (size_t col = 0; col < matrix->cols; col++) {
        double value = matrix_get(matrix, target_row, col) + scalar * matrix_get(matrix, source_row, col);
        matrix_set(matrix, target_row, col, value);
    }
}

void matrix_clean_small_values(Matrix *matrix, double epsilon)
{
    assert(matrix != NULL);

    size_t element_count = matrix->rows * matrix->cols;
    for (size_t i = 0; i < element_count; i++) {
        if (fabs(matrix->data[i]) < epsilon) {
            matrix->data[i] = 0.0;
        }
    }
}

double matrix_normalize_zero(double value)
{
    if (value == 0.0) {
        return 0.0;
    }
    return value;
}

void matrix_print(FILE *output, const Matrix *matrix, size_t variable_count)
{
    assert(output != NULL);
    assert(matrix != NULL);

    bool has_augmented_column = (variable_count < matrix->cols);

    for (size_t row = 0; row < matrix->rows; row++) {
        fprintf(output, "[ ");
        for (size_t col = 0; col < matrix->cols; col++) {
            if (has_augmented_column && col == variable_count) {
                fprintf(output, "| ");
            }

            double value = matrix_normalize_zero(matrix_get(matrix, row, col));
            fprintf(output, "%*.*f ", NUMBER_FIELD_WIDTH, NUMBER_PRECISION, value);
        }
        fprintf(output, "]\n");
    }
}
