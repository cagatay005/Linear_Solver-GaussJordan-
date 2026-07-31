#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "gauss_jordan.h"
#include "input.h"
#include "matrix.h"
#include "output.h"
#include "parser.h"
#include "solution.h"
#include "verification.h"

static void solve_and_report(
    const Matrix *original_augmented,
    size_t variable_count,
    double epsilon,
    bool verbose,
    FILE *output
)
{
    output_print_header(output);

    output_print_section_title(output, 1, "Girilen Denklem Sistemi");
    output_print_equations(output, original_augmented, variable_count);

    output_print_section_title(output, 2, "Baslangic Artirilmis Matrisi");
    fprintf(output, "\n");
    matrix_print(output, original_augmented, variable_count);

    Matrix working;
    if (!matrix_copy(&working, original_augmented)) {
        fprintf(stderr, "Kritik hata: matris kopyalanamadi.\n");
        exit(EXIT_FAILURE);
    }

    GaussJordanResult gj_result;
    bool solved;

    output_print_section_title(output, 3, "Gauss-Jordan Cozum Adimlari");
    if (verbose) {
        fprintf(output, "\n");
        solved = gauss_jordan_solve(&working, variable_count, epsilon, true, output, &gj_result);
    } else {
        fprintf(output, "\n(Sessiz mod secildi, ara adimlar gosterilmiyor.)\n");
        solved = gauss_jordan_solve(&working, variable_count, epsilon, false, output, &gj_result);
    }

    if (!solved) {
        fprintf(stderr, "Kritik hata: Gauss-Jordan eliminasyonu gerceklestirilemedi.\n");
        matrix_destroy(&working);
        exit(EXIT_FAILURE);
    }

    output_print_section_title(output, 4, "Indirgenmis Satir Basamak Matrisi (RREF)");
    fprintf(output, "\n");
    matrix_print(output, &working, variable_count);

    output_print_section_title(output, 5, "Rank Analizi");
    fprintf(output, "\n");
    output_print_rank_analysis(output, &gj_result, variable_count);

    if (gj_result.near_singular_warning) {
        output_print_near_singular_warning(output);
    }

    output_print_section_title(output, 6, "Sistem Turu ve Cozum");
    fprintf(output, "\n");

    if (gj_result.type == SYSTEM_INCONSISTENT) {
        output_print_inconsistent(output, &working, &gj_result, variable_count);
    } else {
        SolutionSet solution;
        if (!solution_build(&working, variable_count, &gj_result, &solution)) {
            fprintf(stderr, "Kritik hata: cozum kumesi olusturulamadi.\n");
            gauss_jordan_result_destroy(&gj_result);
            matrix_destroy(&working);
            exit(EXIT_FAILURE);
        }
        solution_print(output, &solution);

        if (gj_result.type == SYSTEM_UNIQUE) {
            output_print_section_title(output, 7, "Dogrulama");
            fprintf(output, "\n");

            VerificationResult verification;
            if (verification_verify(original_augmented, &solution, epsilon, &verification)) {
                verification_print(output, &verification, epsilon);
                verification_destroy(&verification);
            } else {
                fprintf(stderr, "Uyari: dogrulama gerceklestirilemedi.\n");
            }
        }

        solution_destroy(&solution);
    }

    output_print_section_title(output, 8, "Islem Ozeti");
    fprintf(output, "\n");
    output_print_operation_summary(output, &gj_result, epsilon);

    gauss_jordan_result_destroy(&gj_result);
    matrix_destroy(&working);
}

static bool prompt_and_read_file(Matrix *augmented, size_t *equation_count, size_t *variable_count)
{
    char filename[LINE_BUFFER_SIZE];

    for (;;) {
        printf("Dosya adi: ");

        if (!read_line(filename, sizeof(filename), stdin)) {
            printf("\nGirdi okunamadi, lutfen tekrar deneyin.\n");
            clearerr(stdin);
            continue;
        }

        if (filename[0] == '\0') {
            printf("Hata: Dosya adi bos olamaz.\n");
            continue;
        }

        if (input_read_system_from_file(filename, augmented, equation_count, variable_count)) {
            return true;
        }

        printf("Lutfen gecerli bir dosya adi girin.\n");
    }
}

int main(int argc, char *argv[])
{
    double epsilon = DEFAULT_EPSILON;

    Matrix augmented;
    size_t equation_count = 0;
    size_t variable_count = 0;

    if (argc > 1) {
        if (!input_read_system_from_file(argv[1], &augmented, &equation_count, &variable_count)) {
            fprintf(stderr, "Program sonlandiriliyor: '%s' dosyasi okunamadi.\n", argv[1]);
            return EXIT_FAILURE;
        }
        printf("Sistem '%s' dosyasindan okundu.\n\n", argv[1]);
    } else {
        printf("1 - Klavyeden giris\n");
        printf("2 - Dosyadan giris\n");
        long source_choice = input_read_menu_choice("Seciminiz: ", 1, 2);

        if (source_choice == 1) {
            input_read_dimensions_keyboard(&equation_count, &variable_count);
            augmented = matrix_create(equation_count, variable_count + 1);
            input_read_system_keyboard(&augmented, equation_count, variable_count);
        } else {
            if (!prompt_and_read_file(&augmented, &equation_count, &variable_count)) {
                return EXIT_FAILURE;
            }
        }
    }

    printf("\n1 - Ayrintili cozum\n");
    printf("2 - Yalnizca sonuc\n");
    long mode_choice = input_read_menu_choice("Seciminiz: ", 1, 2);
    bool verbose = (mode_choice == 1);

    solve_and_report(&augmented, variable_count, epsilon, verbose, stdout);

    bool save_report = input_read_yes_no("\nCozum adimlari dosyaya kaydedilsin mi? (e/h): ");

    if (save_report) {
        char filename[LINE_BUFFER_SIZE];
        printf("Rapor dosyasi adi (bos birakilirsa '%s' kullanilir): ", DEFAULT_REPORT_FILENAME);

        if (!read_line(filename, sizeof(filename), stdin)) {
            clearerr(stdin);
            filename[0] = '\0';
        }

        const char *report_filename = (filename[0] == '\0') ? DEFAULT_REPORT_FILENAME : filename;

        FILE *report_file = fopen(report_filename, "w");
        if (report_file == NULL) {
            fprintf(stderr, "Hata: '%s' dosyasina yazilamadi.\n", report_filename);
        } else {
            solve_and_report(&augmented, variable_count, epsilon, true, report_file);
            fclose(report_file);
            printf("Cozum raporu '%s' dosyasina kaydedildi.\n", report_filename);
        }
    }

    matrix_destroy(&augmented);

    return EXIT_SUCCESS;
}
