#ifndef VERIFICATION_H
#define VERIFICATION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "matrix.h"
#include "solution.h"

/*
 * Tek cozumlu bir sistemin, orijinal (elimine edilmemis) artirilmis
 * matriste yerine konularak dogrulanmasinin sonucunu tasir.
 */
typedef struct {
    size_t equation_count;
    double *computed_lhs; /* boyut: equation_count */
    double *expected_rhs; /* boyut: equation_count */
    double *errors;       /* boyut: equation_count, |computed_lhs - expected_rhs| */
    double max_residual;
    bool verified; /* max_residual < epsilon ise true */
} VerificationResult;

/*
 * original_augmented (Gauss-Jordan islemlerinden ONCE alinmis kopya) ve
 * solution (tek cozumlu, yani SYSTEM_UNIQUE) kullanilarak her denklemin
 * sol tarafini yeniden hesaplar ve sag tarafla karsilastirir.
 */
bool verification_verify(
    const Matrix *original_augmented,
    const SolutionSet *solution,
    double epsilon,
    VerificationResult *result
);

/* result icerisinde ayrilan tum dinamik bellegi serbest birakir. */
void verification_destroy(VerificationResult *result);

/* Dogrulama sonucunu okunabilir bicimde output akisina yazdirir. */
void verification_print(FILE *output, const VerificationResult *result, double epsilon);

#endif /* VERIFICATION_H */
