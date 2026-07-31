#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/*
 * Genel amacli, satir-majör (row-major) duzende saklanan dinamik matris
 * yapisi. Tum sayisal islemler double hassasiyetinde yapilir.
 */
typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} Matrix;

/*
 * rows x cols boyutunda, tum elemanlari 0.0 olan bir matris olusturur.
 * Bellek ayirma basarisiz olursa hata mesaji yazdirilir ve program
 * EXIT_FAILURE ile sonlandirilir (kurtarilamaz bir durumdur).
 */
Matrix matrix_create(size_t rows, size_t cols);

/* Matrisin ayirdigi bellegi serbest birakir ve alanlarini sifirlar. */
void matrix_destroy(Matrix *matrix);

/*
 * source matrisini destination'a kopyalar. destination'in daha once
 * matrix_create ile olusturulmus olmasi GEREKMEZ; bu fonksiyon
 * destination'i yeniden ayirir. Basarili olursa true, source NULL ise
 * false doner.
 */
bool matrix_copy(Matrix *destination, const Matrix *source);

/* Belirtilen satir/sutundaki degeri okur. Sinirlar debug modunda (NDEBUG
 * tanimli degilse) assert ile dogrulanir. */
double matrix_get(const Matrix *matrix, size_t row, size_t col);

/* Belirtilen satir/sutuna deger yazar. */
void matrix_set(Matrix *matrix, size_t row, size_t col, double value);

/* row1 ve row2 satirlarinin tum elemanlarini yer degistirir. */
void matrix_swap_rows(Matrix *matrix, size_t row1, size_t row2);

/* row satirinin tum elemanlarini scalar ile carpar. */
void matrix_scale_row(Matrix *matrix, size_t row, double scalar);

/*
 * target_row satirina, source_row satirinin scalar katini ekler:
 * target_row <- target_row + scalar * source_row
 */
void matrix_add_scaled_row(
    Matrix *matrix,
    size_t target_row,
    size_t source_row,
    double scalar
);

/* Mutlak degeri epsilon'dan kucuk olan tum elemanlari 0.0 yapar. Bu ayni
 * zamanda -0.0 gibi negatif sifir degerlerini de temizler. */
void matrix_clean_small_values(Matrix *matrix, double epsilon);

/*
 * Matrisi hizali sekilde ekrana/dosyaya yazdirir. variable_count,
 * matrisin son sutununun "artirilmis" (sabit terim) sutunu olarak ayri
 * gosterilmesi icin kullanilir; variable_count == matrix->cols ise
 * ayrac kullanilmaz (sade bir matris olarak yazdirilir).
 */
void matrix_print(FILE *output, const Matrix *matrix, size_t variable_count);

/*
 * Ekrana yazdirma amaciyla -0.0 gibi negatif sifir degerlerini 0.0'a
 * cevirir. Diger tum degerleri oldugu gibi dondurur. Sayisal hesaplamalar
 * icin degil, yalnizca goruntuleme icin kullanilmalidir.
 */
double matrix_normalize_zero(double value);

#endif /* MATRIX_H */
