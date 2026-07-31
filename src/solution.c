#include "solution.h"

#include <stdint.h>
#include <stdlib.h>

static void fatal_memory_error(const char *context)
{
    fprintf(stderr, "Kritik bellek hatasi: %s icin bellek ayrilamadi.\n", context);
    exit(EXIT_FAILURE);
}

static size_t find_pivot_row_for_column(const GaussJordanResult *gj_result, size_t column)
{
    for (size_t k = 0; k < gj_result->pivot_count; k++) {
        if (gj_result->pivot_columns[k] == column) {
            return k;
        }
    }
    return SIZE_MAX;
}

bool solution_build(
    const Matrix *rref,
    size_t variable_count,
    const GaussJordanResult *gj_result,
    SolutionSet *solution
)
{
    if (rref == NULL || gj_result == NULL || solution == NULL) {
        return false;
    }

    solution->type = gj_result->type;
    solution->variable_count = variable_count;
    solution->free_variable_count = gj_result->free_variable_count;
    solution->free_columns = NULL;
    solution->expressions = NULL;

    if (gj_result->type == SYSTEM_INCONSISTENT) {
        return true;
    }

    if (solution->free_variable_count > 0) {
        solution->free_columns = (size_t *)malloc(solution->free_variable_count * sizeof(size_t));
        if (solution->free_columns == NULL) {
            fatal_memory_error("serbest degisken sutun listesi");
        }

        size_t index = 0;
        for (size_t column = 0; column < variable_count; column++) {
            if (!gj_result->is_pivot_column[column]) {
                solution->free_columns[index] = column;
                index++;
            }
        }
    }

    solution->expressions = (VariableExpression *)malloc(variable_count * sizeof(VariableExpression));
    if (variable_count > 0 && solution->expressions == NULL) {
        fatal_memory_error("degisken ifadeleri");
    }

    for (size_t v = 0; v < variable_count; v++) {
        VariableExpression *expr = &solution->expressions[v];
        expr->parameter_coefficients = NULL;

        if (solution->free_variable_count > 0) {
            expr->parameter_coefficients = (double *)calloc(solution->free_variable_count, sizeof(double));
            if (expr->parameter_coefficients == NULL) {
                fatal_memory_error("parametre katsayilari");
            }
        }

        if (gj_result->is_pivot_column[v]) {
            size_t pivot_index = find_pivot_row_for_column(gj_result, v);
            size_t row = pivot_index; /* i-inci pivot, i-inci satirdadir */

            expr->constant = matrix_get(rref, row, variable_count);

            for (size_t j = 0; j < solution->free_variable_count; j++) {
                double coefficient_in_row = matrix_get(rref, row, solution->free_columns[j]);
                expr->parameter_coefficients[j] = -coefficient_in_row;
            }
        } else {
            expr->constant = 0.0;

            for (size_t j = 0; j < solution->free_variable_count; j++) {
                expr->parameter_coefficients[j] = (solution->free_columns[j] == v) ? 1.0 : 0.0;
            }
        }
    }

    return true;
}

void solution_destroy(SolutionSet *solution)
{
    if (solution == NULL) {
        return;
    }

    if (solution->expressions != NULL) {
        for (size_t v = 0; v < solution->variable_count; v++) {
            free(solution->expressions[v].parameter_coefficients);
        }
        free(solution->expressions);
    }

    free(solution->free_columns);

    solution->expressions = NULL;
    solution->free_columns = NULL;
    solution->variable_count = 0;
    solution->free_variable_count = 0;
}

static void print_term(FILE *output, double coefficient, const char *symbol, bool is_first_term)
{
    double value = matrix_normalize_zero(coefficient);

    if (value == 0.0) {
        return;
    }

    if (is_first_term) {
        if (value < 0.0) {
            fprintf(output, "-%.6f%s", -value, symbol);
        } else {
            fprintf(output, "%.6f%s", value, symbol);
        }
    } else {
        if (value < 0.0) {
            fprintf(output, " - %.6f%s", -value, symbol);
        } else {
            fprintf(output, " + %.6f%s", value, symbol);
        }
    }
}

static void print_expression(FILE *output, const VariableExpression *expr, size_t free_variable_count)
{
    bool any_term_printed = false;
    double constant = matrix_normalize_zero(expr->constant);

    if (constant != 0.0 || free_variable_count == 0) {
        fprintf(output, "%.6f", constant);
        any_term_printed = true;
    }

    for (size_t j = 0; j < free_variable_count; j++) {
        char symbol[16];
        snprintf(symbol, sizeof(symbol), "*t%zu", j + 1);

        if (matrix_normalize_zero(expr->parameter_coefficients[j]) != 0.0) {
            print_term(output, expr->parameter_coefficients[j], symbol, !any_term_printed);
            any_term_printed = true;
        }
    }

    if (!any_term_printed) {
        fprintf(output, "0.000000");
    }
}

void solution_print(FILE *output, const SolutionSet *solution)
{
    if (output == NULL || solution == NULL) {
        return;
    }

    if (solution->type == SYSTEM_INCONSISTENT) {
        return;
    }

    if (solution->type == SYSTEM_UNIQUE) {
        fprintf(output, "Sistem turu: Tek cozumlu sistem\n\n");
        for (size_t v = 0; v < solution->variable_count; v++) {
            fprintf(output, "x%zu = %.6f\n", v + 1, matrix_normalize_zero(solution->expressions[v].constant));
        }
        return;
    }

    fprintf(output, "Sistem turu: Sonsuz cozumlu sistem\n\n");

    fprintf(output, "Serbest degisken%s:\n", solution->free_variable_count > 1 ? "ler" : "");
    for (size_t j = 0; j < solution->free_variable_count; j++) {
        fprintf(output, "x%zu = t%zu\n", solution->free_columns[j] + 1, j + 1);
    }

    fprintf(output, "\nPivot degiskenleri:\n");
    for (size_t v = 0; v < solution->variable_count; v++) {
        fprintf(output, "x%zu = ", v + 1);
        print_expression(output, &solution->expressions[v], solution->free_variable_count);
        fprintf(output, "\n");
    }
}
