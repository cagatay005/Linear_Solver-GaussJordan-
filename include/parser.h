#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/* Sayi ayristirma islemlerinin sonucunu belirten durum kodlari. */
typedef enum {
    PARSE_OK = 0,
    PARSE_ERROR_EMPTY,
    PARSE_ERROR_INVALID,
    PARSE_ERROR_OUT_OF_RANGE,
    PARSE_ERROR_NAN_OR_INF
} ParseStatus;

/*
 * input akisindan bir satir okur (fgets tabanli). Satir sonundaki '\n'
 * ve '\r' karakterlerini temizler. Basari durumunda true, EOF veya
 * hata durumunda false doner.
 */
bool read_line(char *buffer, size_t buffer_size, FILE *input);

/*
 * str icerisindeki metni bir double degere ayristirir. strtod tabanlidir;
 * bosluklar dislanir, fazladan gecersiz karakter, bos girdi, NaN/sonsuz
 * degerler ve tasma durumlari ayri ayri raporlanir. Basarili olursa
 * PARSE_OK doner ve *out_value doldurulur.
 */
ParseStatus parse_double(const char *str, double *out_value);

/*
 * str icerisindeki metni bir long degere ayristirir. Ayni kurallar
 * parse_double icin gecerlidir.
 */
ParseStatus parse_long(const char *str, long *out_value);

/* Bir ParseStatus degerine karsilik gelen okunabilir Turkce mesaji dondurur. */
const char *parse_status_message(ParseStatus status);

#endif /* PARSER_H */
