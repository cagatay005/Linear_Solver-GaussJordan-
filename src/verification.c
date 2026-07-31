#include "verification.h"

#include <math.h>
#include <stdlib.h>

static void fatal_memory_error(const char *context)
{
    fprintf(stderr, "Kritik bellek hatasi: %s icin bellek ayrilamadi.\n", context);
    exit(EXIT_FAILURE);
}

bool verification_verify(
    const Matrix *original_augmented,
    const SolutionSet *solution,
    double epsilon,
    VerificationResult *result
)
{
    if (original_augmented == NULL || solution == NULL || result == NULL) {
        return false;
    }

    if (solution->type != SYSTEM_UNIQUE) {
        return false;
    }

    size_t equation_count = original_augmented->rows;
    size_t variable_count = solution->variable_count;

    result->equation_count = equation_count;
    result->computed_lhs = NULL;
    result->expected_rhs = NULL;
    result->errors = NULL;
    result->max_residual = 0.0;
    result->verified = false;

    if (equation_count == 0) {
        result->verified = true;
        return true;
    }

    result->computed_lhs = (double *)malloc(equation_count * sizeof(double));
    result->expected_rhs = (double *)malloc(equation_count * sizeof(double));
    result->errors = (double *)malloc(equation_count * sizeof(double));

    if (result->computed_lhs == NULL || result->expected_rhs == NULL || result->errors == NULL) {
        fatal_memory_error("dogrulama sonuc dizileri");
    }

    for (size_t row = 0; row < equation_count; row++) {
        double lhs = 0.0;
        for (size_t col = 0; col < variable_count; col++) {
            lhs += matrix_get(original_augmented, row, col) * solution->expressions[col].constant;
        }

        double rhs = matrix_get(original_augmented, row, variable_count);
        double error = fabs(lhs - rhs);

        result->computed_lhs[row] = lhs;
        result->expected_rhs[row] = rhs;
        result->errors[row] = error;

        if (error > result->max_residual) {
            result->max_residual = error;
        }
    }

    result->verified = (result->max_residual < epsilon);

    return true;
}

void verification_destroy(VerificationResult *result)
{
    if (result == NULL) {
        return;
    }

    free(result->computed_lhs);
    free(result->expected_rhs);
    free(result->errors);

    result->computed_lhs = NULL;
    result->expected_rhs = NULL;
    result->errors = NULL;
    result->equation_count = 0;
}

void verification_print(FILE *output, const VerificationResult *result, double epsilon)
{
    if (output == NULL || result == NULL) {
        return;
    }

    fprintf(output, "Cozum dogrulamasi:\n\n");

    for (size_t row = 0; row < result->equation_count; row++) {
        fprintf(output, "Denklem %zu:\n", row + 1);
        fprintf(output, "Hesaplanan sol taraf = %.6f\n", matrix_normalize_zero(result->computed_lhs[row]));
        fprintf(output, "Beklenen sag taraf   = %.6f\n", matrix_normalize_zero(result->expected_rhs[row]));
        fprintf(output, "Hata                  = %.6e\n\n", result->errors[row]);
    }

    fprintf(output, "Maksimum residual hata: %.6e\n", result->max_residual);

    if (result->verified) {
        fprintf(output, "\nCozum basariyla dogrulandi.\n");
    } else {
        fprintf(output, "\nUyari: Cozumdeki residual hata epsilon (%.6e) sinirini asiyor.\n", epsilon);
    }
}
