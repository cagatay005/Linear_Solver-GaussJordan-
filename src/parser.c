#include "parser.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

bool read_line(char *buffer, size_t buffer_size, FILE *input)
{
    if (buffer == NULL || buffer_size == 0 || input == NULL) {
        return false;
    }

    if (fgets(buffer, (int)buffer_size, input) == NULL) {
        buffer[0] = '\0';
        return false;
    }

    size_t length = strlen(buffer);

    /* Satirin tamamen okunup okunmadigini kontrol et; eger satir sonuna
     * '\n' konulmamissa ve dosya sonuna da ulasilmamissa, tampon dolmus
     * demektir. Bu durumda akistaki kalan kismi tuketerek bir sonraki
     * okumanin dogru satirdan baslamasini sagla. */
    if (length > 0 && buffer[length - 1] != '\n') {
        int ch;
        while ((ch = fgetc(input)) != EOF && ch != '\n') {
            /* kalan karakterleri at */
        }
    }

    while (length > 0 && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r')) {
        buffer[length - 1] = '\0';
        length--;
    }

    return true;
}

static bool string_is_blank(const char *str)
{
    for (const char *p = str; *p != '\0'; p++) {
        if (!isspace((unsigned char)*p)) {
            return false;
        }
    }
    return true;
}

ParseStatus parse_double(const char *str, double *out_value)
{
    if (str == NULL || out_value == NULL) {
        return PARSE_ERROR_INVALID;
    }

    if (str[0] == '\0' || string_is_blank(str)) {
        return PARSE_ERROR_EMPTY;
    }

    errno = 0;
    char *end_pointer = NULL;
    double value = strtod(str, &end_pointer);

    if (end_pointer == str) {
        return PARSE_ERROR_INVALID;
    }

    /* Ayristirilan sayidan sonra bosluk disinda karakter kalmamali. */
    while (*end_pointer != '\0' && isspace((unsigned char)*end_pointer)) {
        end_pointer++;
    }
    if (*end_pointer != '\0') {
        return PARSE_ERROR_INVALID;
    }

    if (errno == ERANGE) {
        return PARSE_ERROR_OUT_OF_RANGE;
    }

    bool is_nan = (value != value);
    bool is_inf = (value == HUGE_VAL) || (value == -HUGE_VAL);
    if (is_nan || is_inf) {
        return PARSE_ERROR_NAN_OR_INF;
    }

    *out_value = value;
    return PARSE_OK;
}

ParseStatus parse_long(const char *str, long *out_value)
{
    if (str == NULL || out_value == NULL) {
        return PARSE_ERROR_INVALID;
    }

    if (str[0] == '\0' || string_is_blank(str)) {
        return PARSE_ERROR_EMPTY;
    }

    errno = 0;
    char *end_pointer = NULL;
    long value = strtol(str, &end_pointer, 10);

    if (end_pointer == str) {
        return PARSE_ERROR_INVALID;
    }

    while (*end_pointer != '\0' && isspace((unsigned char)*end_pointer)) {
        end_pointer++;
    }
    if (*end_pointer != '\0') {
        return PARSE_ERROR_INVALID;
    }

    if (errno == ERANGE) {
        return PARSE_ERROR_OUT_OF_RANGE;
    }

    *out_value = value;
    return PARSE_OK;
}

const char *parse_status_message(ParseStatus status)
{
    switch (status) {
        case PARSE_OK:
            return "Gecerli bir sayi.";
        case PARSE_ERROR_EMPTY:
            return "Girdi bos olamaz.";
        case PARSE_ERROR_INVALID:
            return "Girdi gecerli bir sayi degil.";
        case PARSE_ERROR_OUT_OF_RANGE:
            return "Girdi izin verilen sayisal araligin disinda.";
        case PARSE_ERROR_NAN_OR_INF:
            return "NaN veya sonsuz degerlere izin verilmiyor.";
        default:
            return "Bilinmeyen ayristirma hatasi.";
    }
}
