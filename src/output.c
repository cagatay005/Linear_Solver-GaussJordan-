#include "output.h"
#include "config.h"

#include <math.h>
#include <stdbool.h>

void output_print_header(FILE *output)
{
    fprintf(output, "============================================================\n");
    fprintf(output, "              GAUSS-JORDAN DENKLEM COZUCU\n");
    fprintf(output, "============================================================\n");
}

void output_print_section_title(FILE *output, int number, const char *title)
{
    fprintf(output, "\n[%d] %s\n", number, title);
    fprintf(output, "------------------------------------------------------------\n");
}

static void print_equation_term(FILE *output, double coefficient, size_t variable_index, bool *any_term_printed)
{
    double value = matrix_normalize_zero(coefficient);

    if (value == 0.0) {
        return;
    }

    if (!*any_term_printed) {
        if (value < 0.0) {
            fprintf(output, "-");
        }
    } else {
        fprintf(output, value < 0.0 ? " - " : " + ");
    }

    double abs_value = fabs(value);
    if (fabs(abs_value - 1.0) < 1e-9) {
        fprintf(output, "x%zu", variable_index + 1);
    } else {
        fprintf(output, "%.6f*x%zu", abs_value, variable_index + 1);
    }

    *any_term_printed = true;
}

void output_print_equations(FILE *output, const Matrix *augmented, size_t variable_count)
{
    for (size_t row = 0; row < augmented->rows; row++) {
        bool any_term_printed = false;

        for (size_t col = 0; col < variable_count; col++) {
            print_equation_term(output, matrix_get(augmented, row, col), col, &any_term_printed);
        }

        if (!any_term_printed) {
            fprintf(output, "0");
        }

        double rhs = matrix_normalize_zero(matrix_get(augmented, row, variable_count));
        fprintf(output, " = %.6f\n", rhs);
    }
}

void output_print_rank_analysis(FILE *output, const GaussJordanResult *result, size_t variable_count)
{
    fprintf(output, "Matris analizi:\n\n");
    fprintf(output, "rank(A)                 = %zu\n", result->rank_coefficient);
    fprintf(output, "rank([A|b])             = %zu\n", result->rank_augmented);
    fprintf(output, "Pivot sayisi            = %zu\n", result->pivot_count);
    fprintf(output, "Serbest degisken sayisi = %zu\n", variable_count - result->rank_coefficient);
}

void output_print_operation_summary(FILE *output, const GaussJordanResult *result, double epsilon)
{
    fprintf(output, "Islem ozeti:\n\n");
    fprintf(output, "Toplam satir islemi : %zu\n", result->total_operations);
    fprintf(output, "Satir degistirme    : %zu\n", result->row_swaps);
    fprintf(output, "Normalizasyon       : %zu\n", result->row_scalings);
    fprintf(output, "Eliminasyon         : %zu\n", result->row_eliminations);
    fprintf(output, "Pivot sayisi        : %zu\n", result->pivot_count);
    fprintf(output, "Epsilon             : %e\n", epsilon);
}

void output_print_inconsistent(
    FILE *output,
    const Matrix *rref,
    const GaussJordanResult *result,
    size_t variable_count
)
{
    fprintf(output, "Cozum bulunamadi.\n\n");
    fprintf(output, "Cunku indirgenmis matriste su celiskili satir olustu:\n\n");

    size_t row = result->inconsistent_row;

    fprintf(output, "[ ");
    for (size_t col = 0; col < variable_count; col++) {
        double value = matrix_normalize_zero(matrix_get(rref, row, col));
        fprintf(output, "%*.*f ", NUMBER_FIELD_WIDTH, NUMBER_PRECISION, value);
    }
    fprintf(output, "| ");
    double constant = matrix_normalize_zero(matrix_get(rref, row, variable_count));
    fprintf(output, "%*.*f ]\n\n", NUMBER_FIELD_WIDTH, NUMBER_PRECISION, constant);

    fprintf(output, "Bu satir matematiksel olarak 0 = %.6f anlamina gelir.\n", constant);
    fprintf(output, "Dolayisiyla sistem tutarsizdir.\n");
}

void output_print_near_singular_warning(FILE *output)
{
    fprintf(output, "\nUyari: Sistem sayisal olarak kararsiz olabilir.\n");
    fprintf(output, "Cok kucuk bir pivot degeri kullanildi.\n");
}
