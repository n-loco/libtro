#ifndef TRO_STRING_H_
#define TRO_STRING_H_

#include "tro/private/api.h"

#include <stddef.h>
#include <stdint.h>

TRO__C_API_BEGIN

/**
 * Representa um único byte de
 * uma string em UTF-8.
 */
typedef uint8_t tro_u8code;

/**
 * Define o máximo que um `rune`/code point
 * pode chegar em tamanho em UTF-8.
 */
#define TRO_MULTI_U8CODE_MAX 4

/**
 * Representa um único surrogate
 * (número de 16 bits) de uma
 * string em UTF-16.
 */
typedef uint16_t tro_u16code;

/**
 * Define o máximo que um `rune`/code point
 * pode chegar em tamanho em UTF-16.
 */
#define TRO_MULTI_U16CODE_MAX 2

/**
 * Representa um code point Unicode
 * (e também pode ser usado para criar
 * string UTF-32).
 */
typedef uint32_t tro_urune;

/**
 * Define o code point Unicode máximo.
 */
#define TRO_URUNE_MAX 0x10FFFF

TRO__API size_t tro_urune_to_u8codes(tro_urune rune, tro_u8code *out);

TRO__API size_t tro_u8codes_to_urune(const tro_u8code *seq, tro_urune *out);

TRO__API size_t tro_urune_to_u16codes(tro_urune rune, tro_u16code *out);

TRO__API size_t tro_str8_urune_len(const char *str, size_t str_l);

typedef uint16_t tro_char16;

static inline size_t tro_strulen(const char *str)
{
	return tro_str8_urune_len(str, 0);
}

TRO__API size_t tro_conv_str_to_str16(const char *in, size_t in_len,
                                      tro_char16 *out, size_t out_cap);

TRO__API tro_char16 *tro_cnvlloc_str_to_str16(const char *in, size_t in_len,
                                              size_t *len_out);

TRO__C_API_END

#endif // TRO_STRING_H_
