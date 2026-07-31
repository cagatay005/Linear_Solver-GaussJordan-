#ifndef SOLUTION_H
#define SOLUTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "gauss_jordan.h"
#include "matrix.h"

/*
 * Bir degiskenin parametrik ifadesini temsil eder:
 *   x_i = constant + sum_k parameter_coefficients[k] * t_(k+1)
 * free_variable_count == 0 ise (tek cozum) ifade sadece constant'tan
 * ibarettir ve parameter_coefficients kullanilmaz.
 */
typedef struct {
    double constant;
    double *parameter_coefficients; /* boyut: free_variable_count */
} VariableExpression;

/* Bir dogrusal denklem sisteminin tam cozum kumesini temsil eder. */
typedef struct {
    SystemType type;
    size_t variable_count;
    size_t free_variable_count;
    size_t *free_columns;           /* boyut: free_variable_count, artan sirada */
    VariableExpression *expressions; /* boyut: variable_count, sadece type != SYSTEM_INCONSISTENT icin gecerli */
} SolutionSet;

/*
 * rref (RREF bicimindeki artirilmis matris) ve gj_result kullanilarak
 * cozum kumesini olusturur. Sistem SYSTEM_INCONSISTENT ise expressions
 * ve free_columns NULL birakilir. Basarili olursa true doner.
 */
bool solution_build(
    const Matrix *rref,
    size_t variable_count,
    const GaussJordanResult *gj_result,
    SolutionSet *solution
);

/* solution icerisinde ayrilan tum dinamik bellegi serbest birakir. */
void solution_destroy(SolutionSet *solution);

/*
 * Cozum kumesini okunabilir bicimde output akisina yazdirir. Sistem
 * turune gore (tek cozum / sonsuz cozum) uygun formatta gosterim yapar.
 * SYSTEM_INCONSISTENT durumu bu fonksiyon tarafindan ele alinmaz; cagiran
 * taraf o durumu ayrica raporlamalidir.
 */
void solution_print(FILE *output, const SolutionSet *solution);

#endif /* SOLUTION_H */
