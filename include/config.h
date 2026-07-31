#ifndef CONFIG_H
#define CONFIG_H

/* Sifira yakin degerlerin karsilastirilmasi icin kullanilan varsayilan epsilon. */
#define DEFAULT_EPSILON 1e-10

/* Klavye/dosya satir okuma tamponunun boyutu. */
#define LINE_BUFFER_SIZE 1024

/* Bir sayinin ekrana yazdirilirken kullanacagi genislik ve hassasiyet. */
#define NUMBER_FIELD_WIDTH 12
#define NUMBER_PRECISION 6

/* Cozum raporunun varsayilan dosya adi. */
#define DEFAULT_REPORT_FILENAME "solution_report.txt"

/* Kabul edilebilir maksimum denklem / degisken sayisi (asiri buyuk
 * girislerde tasmayi ve asiri bellek kullanimini engellemek icin). */
#define MAX_EQUATION_COUNT 1000
#define MAX_VARIABLE_COUNT 1000

/* Bir pivot elemaninin mutlak degeri bu esigin altindaysa (fakat epsilon'un
 * ustundeyse) sistemin sayisal olarak kararsiz olabilecegine dair bir
 * uyari yazdirilir. */
#define NEAR_SINGULAR_THRESHOLD 1e-6

/* Satir islemleri icin baslangic dinamik dizi kapasitesi. */
#define INITIAL_OPERATION_CAPACITY 16

#endif /* CONFIG_H */
