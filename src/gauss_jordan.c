#include "gauss_jordan.h"
#include "config.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

static void fatal_memory_error(const char *context)
{
    fprintf(stderr, "Kritik bellek hatasi: %s icin bellek ayrilamadi.\n", context);
    exit(EXIT_FAILURE);
}

void gauss_jordan_result_init(GaussJordanResult *result)
{
    result->type = SYSTEM_UNIQUE;

    result->rank_coefficient = 0;
    result->rank_augmented = 0;
    result->pivot_count = 0;
    result->free_variable_count = 0;

    result->pivot_columns = NULL;
    result->is_pivot_column = NULL;

    result->total_operations = 0;
    result->row_swaps = 0;
    result->row_scalings = 0;
    result->row_eliminations = 0;

    result->operations = NULL;
    result->operations_capacity = 0;

    result->near_singular_warning = false;

    result->has_inconsistent_row = false;
    result->inconsistent_row = 0;
}

void gauss_jordan_result_destroy(GaussJordanResult *result)
{
    if (result == NULL) {
        return;
    }

    free(result->pivot_columns);
    free(result->is_pivot_column);
    free(result->operations);

    result->pivot_columns = NULL;
    result->is_pivot_column = NULL;
    result->operations = NULL;
    result->operations_capacity = 0;
    result->total_operations = 0;
}

static void record_operation(
    GaussJordanResult *result,
    RowOperationType type,
    size_t target_row,
    size_t source_row,
    double scalar
)
{
    if (result->total_operations >= result->operations_capacity) {
        size_t new_capacity = (result->operations_capacity == 0)
            ? INITIAL_OPERATION_CAPACITY
            : result->operations_capacity * 2;

        RowOperation *new_operations = (RowOperation *)realloc(
            result->operations,
            new_capacity * sizeof(RowOperation)
        );

        if (new_operations == NULL) {
            fatal_memory_error("satir islemi kaydi");
        }

        result->operations = new_operations;
        result->operations_capacity = new_capacity;
    }

    RowOperation *op = &result->operations[result->total_operations];
    op->type = type;
    op->target_row = target_row;
    op->source_row = source_row;
    op->scalar = scalar;

    result->total_operations++;

    switch (type) {
        case ROW_OPERATION_SWAP:
            result->row_swaps++;
            break;
        case ROW_OPERATION_SCALE:
            result->row_scalings++;
            break;
        case ROW_OPERATION_ADD_SCALED:
            result->row_eliminations++;
            break;
    }
}

static void print_operation(FILE *output, const RowOperation *op)
{
    switch (op->type) {
        case ROW_OPERATION_SWAP:
            fprintf(output, "R%zu <-> R%zu\n", op->target_row + 1, op->source_row + 1);
            break;
        case ROW_OPERATION_SCALE: {
            double divisor = 1.0 / op->scalar;
            fprintf(output, "R%zu <- R%zu / %.6f\n", op->target_row + 1, op->target_row + 1, divisor);
            break;
        }
        case ROW_OPERATION_ADD_SCALED: {
            if (op->scalar >= 0.0) {
                fprintf(
                    output, "R%zu <- R%zu + %.6f * R%zu\n",
                    op->target_row + 1, op->target_row + 1, op->scalar, op->source_row + 1
                );
            } else {
                fprintf(
                    output, "R%zu <- R%zu - %.6f * R%zu\n",
                    op->target_row + 1, op->target_row + 1, -op->scalar, op->source_row + 1
                );
            }
            break;
        }
    }
}

static size_t find_pivot_row(const Matrix *matrix, size_t start_row, size_t column, double epsilon, double *out_best_value)
{
    size_t best_row = start_row;
    double best_abs_value = fabs(matrix_get(matrix, start_row, column));

    for (size_t row = start_row + 1; row < matrix->rows; row++) {
        double abs_value = fabs(matrix_get(matrix, row, column));
        if (abs_value > best_abs_value) {
            best_abs_value = abs_value;
            best_row = row;
        }
    }

    *out_best_value = matrix_get(matrix, best_row, column);
    return (best_abs_value < epsilon) ? SIZE_MAX : best_row;
}

bool gauss_jordan_solve(
    Matrix *augmented_matrix,
    size_t variable_count,
    double epsilon,
    bool verbose,
    FILE *output,
    GaussJordanResult *result
)
{
    if (augmented_matrix == NULL || result == NULL) {
        return false;
    }

    if (variable_count == 0 || augmented_matrix->cols != variable_count + 1) {
        return false;
    }

    gauss_jordan_result_init(result);

    size_t equation_count = augmented_matrix->rows;
    size_t max_pivots = (equation_count < variable_count) ? equation_count : variable_count;

    result->is_pivot_column = (bool *)calloc(variable_count, sizeof(bool));
    if (result->is_pivot_column == NULL) {
        fatal_memory_error("pivot sutun bayraklari");
    }

    result->pivot_columns = (size_t *)malloc(max_pivots * sizeof(size_t));
    if (max_pivots > 0 && result->pivot_columns == NULL) {
        fatal_memory_error("pivot sutun listesi");
    }

    size_t pivot_row = 0;
    size_t step_number = 0;
    bool verbose_output = verbose && (output != NULL);

    for (size_t column = 0; column < variable_count && pivot_row < equation_count; column++) {
        double best_value = 0.0;
        size_t best_row = find_pivot_row(augmented_matrix, pivot_row, column, epsilon, &best_value);

        if (verbose_output) {
            fprintf(output, "------------------------------------------------------------\n");
            fprintf(output, "PIVOT %zu - Sutun %zu\n", result->pivot_count + 1, column + 1);
            fprintf(output, "------------------------------------------------------------\n");
            fprintf(output, "Pivot sutunu: %zu\n", column + 1);
        }

        if (best_row == SIZE_MAX) {
            if (verbose_output) {
                fprintf(output, "Bu sutunda uygun bir pivot bulunamadi (serbest degisken sutunu).\n\n");
            }
            continue;
        }

        if (verbose_output) {
            fprintf(
                output, "En uygun pivot: %zu. satirdaki %.6f degeri\n\n",
                best_row + 1, matrix_normalize_zero(best_value)
            );
        }

        if (fabs(best_value) < NEAR_SINGULAR_THRESHOLD) {
            result->near_singular_warning = true;
        }

        if (best_row != pivot_row) {
            matrix_swap_rows(augmented_matrix, pivot_row, best_row);
            record_operation(result, ROW_OPERATION_SWAP, pivot_row, best_row, 0.0);
            step_number++;

            if (verbose_output) {
                fprintf(output, "Adim %zu: Pivot satiri ile en uygun satir degistiriliyor.\n", step_number);
                print_operation(output, &result->operations[result->total_operations - 1]);
                fprintf(output, "\n");
                matrix_print(output, augmented_matrix, variable_count);
                fprintf(output, "\n");
            }
        }

        double pivot_value = matrix_get(augmented_matrix, pivot_row, column);
        if (fabs(pivot_value - 1.0) >= epsilon) {
            double scale_factor = 1.0 / pivot_value;
            matrix_scale_row(augmented_matrix, pivot_row, scale_factor);
            record_operation(result, ROW_OPERATION_SCALE, pivot_row, pivot_row, scale_factor);
            step_number++;

            if (verbose_output) {
                fprintf(output, "Adim %zu: Pivot satiri normalize ediliyor.\n", step_number);
                print_operation(output, &result->operations[result->total_operations - 1]);
                fprintf(output, "\n");
                matrix_print(output, augmented_matrix, variable_count);
                fprintf(output, "\n");
            }
        }

        for (size_t row = 0; row < equation_count; row++) {
            if (row == pivot_row) {
                continue;
            }

            double factor = matrix_get(augmented_matrix, row, column);
            if (fabs(factor) < epsilon) {
                continue;
            }

            double scalar = -factor;
            matrix_add_scaled_row(augmented_matrix, row, pivot_row, scalar);
            record_operation(result, ROW_OPERATION_ADD_SCALED, row, pivot_row, scalar);
            step_number++;

            if (verbose_output) {
                fprintf(
                    output, "Adim %zu: Sutun %zu icindeki R%zu elemani sifirlaniyor.\n",
                    step_number, column + 1, row + 1
                );
                print_operation(output, &result->operations[result->total_operations - 1]);
                fprintf(output, "\n");
                matrix_print(output, augmented_matrix, variable_count);
                fprintf(output, "\n");
            }
        }

        matrix_clean_small_values(augmented_matrix, epsilon);

        result->is_pivot_column[column] = true;
        result->pivot_columns[result->pivot_count] = column;
        result->pivot_count++;
        pivot_row++;
    }

    matrix_clean_small_values(augmented_matrix, epsilon);

    result->rank_coefficient = result->pivot_count;
    result->free_variable_count = variable_count - result->pivot_count;

    bool has_inconsistent_row = false;
    for (size_t row = pivot_row; row < equation_count; row++) {
        double constant = matrix_get(augmented_matrix, row, variable_count);
        if (fabs(constant) >= epsilon) {
            has_inconsistent_row = true;
            result->has_inconsistent_row = true;
            result->inconsistent_row = row;
            break;
        }
    }

    result->rank_augmented = result->rank_coefficient + (has_inconsistent_row ? 1 : 0);

    if (has_inconsistent_row) {
        result->type = SYSTEM_INCONSISTENT;
    } else if (result->rank_coefficient == variable_count) {
        result->type = SYSTEM_UNIQUE;
    } else {
        result->type = SYSTEM_INFINITE;
    }

    return true;
}
