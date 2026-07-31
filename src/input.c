#include "input.h"
#include "config.h"
#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

long input_read_menu_choice(const char *prompt, long min_value, long max_value)
{
    char buffer[LINE_BUFFER_SIZE];

    for (;;) {
        printf("%s", prompt);

        if (!read_line(buffer, sizeof(buffer), stdin)) {
            printf("\nGirdi okunamadi, lutfen tekrar deneyin.\n");
            clearerr(stdin);
            continue;
        }

        long value = 0;
        ParseStatus status = parse_long(buffer, &value);

        if (status != PARSE_OK) {
            printf("Hata: %s Lutfen %ld ile %ld arasinda bir tam sayi girin.\n", parse_status_message(status), min_value, max_value);
            continue;
        }

        if (value < min_value || value > max_value) {
            printf("Hata: Deger %ld ile %ld arasinda olmalidir.\n", min_value, max_value);
            continue;
        }

        return value;
    }
}

double input_read_double(const char *prompt)
{
    char buffer[LINE_BUFFER_SIZE];

    for (;;) {
        printf("%s", prompt);

        if (!read_line(buffer, sizeof(buffer), stdin)) {
            printf("\nGirdi okunamadi, lutfen tekrar deneyin.\n");
            clearerr(stdin);
            continue;
        }

        double value = 0.0;
        ParseStatus status = parse_double(buffer, &value);

        if (status != PARSE_OK) {
            printf("Hata: %s Lutfen gecerli bir sayi girin.\n", parse_status_message(status));
            continue;
        }

        return value;
    }
}

bool input_read_yes_no(const char *prompt)
{
    char buffer[LINE_BUFFER_SIZE];

    for (;;) {
        printf("%s", prompt);

        if (!read_line(buffer, sizeof(buffer), stdin)) {
            printf("\nGirdi okunamadi, lutfen tekrar deneyin.\n");
            clearerr(stdin);
            continue;
        }

        size_t length = strlen(buffer);
        size_t start = 0;
        while (start < length && isspace((unsigned char)buffer[start])) {
            start++;
        }

        if (start >= length) {
            printf("Hata: Lutfen 'e' veya 'h' girin.\n");
            continue;
        }

        char answer = (char)tolower((unsigned char)buffer[start]);

        if (answer == 'e') {
            return true;
        }
        if (answer == 'h') {
            return false;
        }

        printf("Hata: Lutfen 'e' (evet) veya 'h' (hayir) girin.\n");
    }
}

void input_read_dimensions_keyboard(size_t *equation_count, size_t *variable_count)
{
    long m = input_read_menu_choice("Denklem sayisi: ", 1, MAX_EQUATION_COUNT);
    long n = input_read_menu_choice("Degisken sayisi: ", 1, MAX_VARIABLE_COUNT);

    *equation_count = (size_t)m;
    *variable_count = (size_t)n;
}

void input_read_system_keyboard(Matrix *augmented, size_t equation_count, size_t variable_count)
{
    char prompt[LINE_BUFFER_SIZE];

    for (size_t row = 0; row < equation_count; row++) {
        printf("\n%zu. denklemin katsayilarini girin:\n", row + 1);

        for (size_t col = 0; col < variable_count; col++) {
            snprintf(prompt, sizeof(prompt), "x%zu: ", col + 1);
            double value = input_read_double(prompt);
            matrix_set(augmented, row, col, value);
        }

        double constant = input_read_double("Sabit terim: ");
        matrix_set(augmented, row, variable_count, constant);
    }
}

static bool count_tokens(char *line, size_t *token_count)
{
    *token_count = 0;
    char *token = strtok(line, " \t");
    while (token != NULL) {
        (*token_count)++;
        token = strtok(NULL, " \t");
    }
    return true;
}

bool input_read_system_from_file(
    const char *filename,
    Matrix *augmented,
    size_t *equation_count,
    size_t *variable_count
)
{
    if (filename == NULL || augmented == NULL || equation_count == NULL || variable_count == NULL) {
        return false;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Hata: '%s' dosyasi acilamadi.\n", filename);
        return false;
    }

    char buffer[LINE_BUFFER_SIZE];
    char buffer_copy[LINE_BUFFER_SIZE];

    if (!read_line(buffer, sizeof(buffer), file)) {
        fprintf(stderr, "Hata: Dosya bos veya okunamiyor.\n");
        fclose(file);
        return false;
    }

    strncpy(buffer_copy, buffer, sizeof(buffer_copy) - 1);
    buffer_copy[sizeof(buffer_copy) - 1] = '\0';

    size_t token_count = 0;
    count_tokens(buffer_copy, &token_count);

    if (token_count != 2) {
        fprintf(stderr, "Hata: Dosyanin ilk satiri 'denklem_sayisi degisken_sayisi' biciminde iki sayi icermelidir.\n");
        fclose(file);
        return false;
    }

    char *token1 = strtok(buffer, " \t");
    char *token2 = strtok(NULL, " \t");

    long m = 0;
    long n = 0;
    if (parse_long(token1, &m) != PARSE_OK || parse_long(token2, &n) != PARSE_OK) {
        fprintf(stderr, "Hata: Ilk satirdaki denklem/degisken sayisi gecersiz.\n");
        fclose(file);
        return false;
    }

    if (m < 1 || m > MAX_EQUATION_COUNT || n < 1 || n > MAX_VARIABLE_COUNT) {
        fprintf(stderr, "Hata: Denklem sayisi ve degisken sayisi 1 ile %d arasinda olmalidir.\n", MAX_EQUATION_COUNT);
        fclose(file);
        return false;
    }

    size_t rows = (size_t)m;
    size_t cols = (size_t)n;

    Matrix matrix = matrix_create(rows, cols + 1);

    for (size_t row = 0; row < rows; row++) {
        if (!read_line(buffer, sizeof(buffer), file)) {
            fprintf(stderr, "Hata: Dosyada %zu. denklem icin satir bulunamadi (beklenen %zu satir).\n", row + 1, rows);
            matrix_destroy(&matrix);
            fclose(file);
            return false;
        }

        strncpy(buffer_copy, buffer, sizeof(buffer_copy) - 1);
        buffer_copy[sizeof(buffer_copy) - 1] = '\0';

        size_t row_token_count = 0;
        count_tokens(buffer_copy, &row_token_count);

        if (row_token_count != cols + 1) {
            fprintf(
                stderr,
                "Hata: %zu. satirda %zu sayi bekleniyordu, %zu sayi bulundu.\n",
                row + 1, cols + 1, row_token_count
            );
            matrix_destroy(&matrix);
            fclose(file);
            return false;
        }

        char *field = strtok(buffer, " \t");
        for (size_t col = 0; col < cols + 1; col++) {
            double value = 0.0;
            ParseStatus status = parse_double(field, &value);

            if (status != PARSE_OK) {
                fprintf(
                    stderr,
                    "Hata: %zu. satir, %zu. deger gecersiz (%s)\n",
                    row + 1, col + 1, parse_status_message(status)
                );
                matrix_destroy(&matrix);
                fclose(file);
                return false;
            }

            matrix_set(&matrix, row, col, value);
            field = strtok(NULL, " \t");
        }
    }

    fclose(file);

    *augmented = matrix;
    *equation_count = rows;
    *variable_count = cols;

    return true;
}
