#ifndef GAUSS_JORDAN_H
#define GAUSS_JORDAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "matrix.h"

/* Bir dogrusal denklem sisteminin cozum turu. */
typedef enum {
    SYSTEM_UNIQUE,
    SYSTEM_INFINITE,
    SYSTEM_INCONSISTENT
} SystemType;

/* Gauss-Jordan eliminasyonunda gerceklestirilebilecek temel satir islemleri. */
typedef enum {
    ROW_OPERATION_SWAP,
    ROW_OPERATION_SCALE,
    ROW_OPERATION_ADD_SCALED
} RowOperationType;

/*
 * Tek bir temel satir islemini temsil eder.
 *   ROW_OPERATION_SWAP:       target_row <-> source_row
 *   ROW_OPERATION_SCALE:      target_row <- target_row * scalar
 *   ROW_OPERATION_ADD_SCALED: target_row <- target_row + scalar * source_row
 */
typedef struct {
    RowOperationType type;
    size_t target_row;
    size_t source_row;
    double scalar;
} RowOperation;

/*
 * Gauss-Jordan eliminasyonunun tam sonucunu tasiyan yapi. Cagiran taraf
 * gauss_jordan_result_init ile baslatmali ve gauss_jordan_result_destroy
 * ile serbest birakmalidir.
 */
typedef struct {
    SystemType type;

    size_t rank_coefficient;
    size_t rank_augmented;
    size_t pivot_count;
    size_t free_variable_count;

    size_t *pivot_columns;     /* boyut: pivot_count, satir sirasina gore pivot sutunlari */
    bool *is_pivot_column;     /* boyut: variable_count */

    size_t total_operations;
    size_t row_swaps;
    size_t row_scalings;
    size_t row_eliminations;

    RowOperation *operations;
    size_t operations_capacity;

    bool near_singular_warning;

    bool has_inconsistent_row;
    size_t inconsistent_row;
} GaussJordanResult;

/* result yapisini gecerli/bos bir baslangic durumuna getirir. */
void gauss_jordan_result_init(GaussJordanResult *result);

/* result icerisinde ayrilan tum dinamik bellegi serbest birakir. */
void gauss_jordan_result_destroy(GaussJordanResult *result);

/*
 * augmented_matrix uzerinde Gauss-Jordan eliminasyonunu (partial pivoting
 * ile) gerceklestirir ve matrisi yerinde RREF (indirgenmis satir basamak)
 * bicimine donusturur. variable_count, artirilmis matrisin son sutunu
 * haric kac sutununun degisken oldugunu belirtir (augmented_matrix->cols
 * == variable_count + 1 olmalidir).
 *
 * verbose true ise, her satir islemi ve o islemden sonraki matris output
 * akisina yazdirilir. output NULL olabilir; bu durumda verbose etkisizdir.
 *
 * Basarili tamamlanirsa true doner ve result doldurulur.
 */
bool gauss_jordan_solve(
    Matrix *augmented_matrix,
    size_t variable_count,
    double epsilon,
    bool verbose,
    FILE *output,
    GaussJordanResult *result
);

#endif /* GAUSS_JORDAN_H */
