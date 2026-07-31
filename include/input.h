#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <stddef.h>

#include "matrix.h"

/*
 * Kullanicidan bir menu secimi (tam sayi) okur; gecersiz girdi (sayi
 * olmayan metin, aralik disi deger) durumunda hatayi aciklar ve tekrar
 * sorar. min_value ve max_value (dahil) kabul edilen araligi belirler.
 */
long input_read_menu_choice(const char *prompt, long min_value, long max_value);

/*
 * Kullanicidan tek bir double deger okur (gecersiz girdide tekrar sorar).
 */
double input_read_double(const char *prompt);

/*
 * Kullanicidan evet/hayir (e/h) yaniti okur. true == evet.
 */
bool input_read_yes_no(const char *prompt);

/*
 * Kullanicidan denklem sayisini ve degisken sayisini okur. Sifir, negatif
 * veya asiri buyuk (MAX_EQUATION_COUNT / MAX_VARIABLE_COUNT ustu)
 * degerlerde tekrar sorar.
 */
void input_read_dimensions_keyboard(size_t *equation_count, size_t *variable_count);

/*
 * Klavyeden, satir satir, her denklemin katsayilarini ve sabit terimini
 * okur. augmented, boyutu (equation_count x (variable_count + 1)) olacak
 * sekilde ONCEDEN olusturulmus olmalidir.
 */
void input_read_system_keyboard(Matrix *augmented, size_t equation_count, size_t variable_count);

/*
 * Belirtilen dosyadan bir dogrusal denklem sistemini okur. Dosyanin ilk
 * satiri "denklem_sayisi degisken_sayisi" iceriklidir, sonraki her satir
 * bir denklemin katsayilarini ve sabit terimini icerir. Basarili olursa
 * true doner, augmented matrisi olusturulmus olarak birakir (cagiran
 * taraf matrix_destroy ile serbest birakmalidir). Basarisiz olursa false
 * doner ve hata mesaji stderr'e yazdirilir.
 */
bool input_read_system_from_file(
    const char *filename,
    Matrix *augmented,
    size_t *equation_count,
    size_t *variable_count
);

#endif /* INPUT_H */
