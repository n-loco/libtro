#ifndef TRO_STRING_H_
#define TRO_STRING_H_

#include "tro/private/api.h"

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

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

#define TRO_URUNE_IS_VALID(rune)                                               \
	((rune < 0xD800 || 0xDFFF < rune) && rune <= TRO_URUNE_MAX)

TRO__API size_t tro_urune_to_u8codes(tro_urune rune, tro_u8code *out);

TRO__API size_t tro_u8codes_to_urune(const tro_u8code *seq, size_t seqlen,
                                     tro_urune *out);

TRO__API size_t tro_urune_to_u16codes(tro_urune rune, tro_u16code *out);

TRO__API size_t tro_u16codes_to_urune(const tro_u16code *seq, size_t seqlen,
                                      tro_urune *out);

static inline size_t tro_u8codes_to_u16codes(const tro_u8code *seq,
                                             size_t seqlen, tro_u16code *out,
                                             size_t *outlen)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(seq, seqlen, &rune);
	*outlen     = tro_urune_to_u16codes(rune, out);
	return read;
}

static inline size_t tro_u16codes_to_u8codes(const tro_u16code *seq,
                                             size_t seqlen, tro_u8code *out,
                                             size_t *outlen)
{
	tro_urune rune;
	size_t read = tro_u16codes_to_urune(seq, seqlen, &rune);
	*outlen     = tro_urune_to_u8codes(rune, out);
	return read;
}

TRO__API size_t tro_str8_urune_len(const char *str, size_t strl);

TRO__API size_t tro_str16_urune_len(const char16_t *str, size_t strl);

static inline size_t tro_strulen(const char *str)
{
	return tro_str8_urune_len(str, 0);
}

static inline size_t tro_str16ulen(const char16_t *str)
{
	return tro_str16_urune_len(str, 0);
}

TRO__API size_t tro_conv_str_to_str16(const char *in, size_t inlen,
                                      char16_t *out, size_t outcap);

TRO__API size_t tro_conv_str16_to_str(const char16_t *in, size_t inlen,
                                      char *out, size_t outcap);

TRO__API char16_t *tro_cnvlloc_str_to_str16(const char *in, size_t inlen,
                                            size_t *lenout);

TRO__API char *tro_cnvlloc_str16_to_str(const char16_t *in, size_t inlen,
                                        size_t *lenout);

static inline size_t tro_str16len(const char16_t *str)
{
	size_t len = 0;
	while (*str) {
		len++;
		str++;
	}
	return len;
}

TRO__C_API_END

#endif // TRO_STRING_H_
