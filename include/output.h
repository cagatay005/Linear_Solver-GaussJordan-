#ifndef OUTPUT_H
#define OUTPUT_H

#include <stddef.h>
#include <stdio.h>

#include "gauss_jordan.h"
#include "matrix.h"

/* Programin ana basligini yazdirir. */
void output_print_header(FILE *output);

/* "[N] Baslik" bicimindeki bir bolum basligi yazdirir. */
void output_print_section_title(FILE *output, int number, const char *title);

/*
 * Artirilmis matrisi "a11*x1 + a12*x2 + ... = b1" biciminde okunabilir
 * denklemler olarak yazdirir.
 */
void output_print_equations(FILE *output, const Matrix *augmented, size_t variable_count);

/* Rank analizini (rank(A), rank([A|b]), pivot sayisi, serbest degisken sayisi) yazdirir. */
void output_print_rank_analysis(FILE *output, const GaussJordanResult *result, size_t variable_count);

/* Toplam islem sayilari ve epsilon degerini iceren islem ozetini yazdirir. */
void output_print_operation_summary(FILE *output, const GaussJordanResult *result, double epsilon);

/*
 * Sistem SYSTEM_INCONSISTENT oldugunda, celiskili satiri (0 = c bicimindeki
 * satiri) rref matrisinden bulup aciklayici bir mesajla birlikte yazdirir.
 */
void output_print_inconsistent(
    FILE *output,
    const Matrix *rref,
    const GaussJordanResult *result,
    size_t variable_count
);

/* Sayisal kararsizlik uyarisini yazdirir (yalnizca result->near_singular_warning true ise cagirilmalidir). */
void output_print_near_singular_warning(FILE *output);

#endif /* OUTPUT_H */
