#ifndef TRO_STRING_H_
#define TRO_STRING_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>
#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif

#include "tro/dybuffer.h"

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

TRO__API size_t tro_str16len(const char16_t *str);

TRO__API size_t tro_str16nlen(const char16_t *str, size_t maxlen);

TRO__API int tro_str16cmp(const char16_t *s1, const char16_t *s2);

TRO__API int tro_str16ncmp(const char16_t *s1, const char16_t *s2,
                           size_t maxlen);

typedef struct tro_strdybuf tro_strdybuf;
TRO__API const tro_dybuffer_i *const tro_strdybuf_dybuffer_vt;
#define tro_strdybuf_dybuffer(buf)                                             \
	((tro_dybuffer_obj){                                                   \
	    .obj    = (void *)buf,                                             \
	    .vtable = tro_strdybuf_dybuffer_vt,                                \
	})

TRO__API tro_strdybuf *tro_create_strdybuf(size_t pagecap);

TRO__API void tro_destroy_strdybuf(tro_strdybuf *buf);

TRO__API bool tro_strdybuf_writes(tro_strdybuf *buf, const char *data,
                                  size_t datal);

TRO__API bool tro_strdybuf_writes16(tro_strdybuf *buf, const char16_t *data,
                                    size_t datal);

TRO__API bool tro_strdybuf_writeb(tro_strdybuf *buf, const uint8_t *data,
                                  size_t datal);

TRO__API bool tro_strdybuf_writec(tro_strdybuf *buf, uint32_t c32);

TRO__API size_t tro_strdybuf_get(tro_strdybuf *buf, char *out, size_t outcap);

TRO__API char *tro_strdybuf_getlloc(tro_strdybuf *buf, size_t *outlen);

typedef struct tro_str16dybuf tro_str16dybuf;
TRO__API const tro_dybuffer_i *const tro_str16dybuf_dybuffer_vt;
#define tro_str16dybuf_dybuffer(buf)                                           \
	((tro_dybuffer_obj){                                                   \
	    .obj    = (void *)buf,                                             \
	    .vtable = tro_str16dybuf_dybuffer_vt,                              \
	})

TRO__API tro_str16dybuf *tro_create_str16dybuf(size_t pagecap);

TRO__API void tro_destroy_str16dybuf(tro_str16dybuf *buf);

TRO__API bool tro_str16dybuf_writes(tro_str16dybuf *buf, const char *data,
                                    size_t datal);

TRO__API bool tro_str16dybuf_writes16(tro_str16dybuf *buf, const char16_t *data,
                                      size_t datal);

TRO__API bool tro_str16dybuf_writeb(tro_str16dybuf *buf, const uint8_t *data,
                                    size_t datal);

TRO__API bool tro_str16dybuf_writec(tro_str16dybuf *buf, uint32_t c32);

TRO__API size_t tro_str16dybuf_get(tro_str16dybuf *buf, char16_t *out, size_t outcap);

TRO__API char16_t *tro_str16dybuf_getlloc(tro_str16dybuf *buf, size_t *outlen);

TRO__C_API_END

#endif // TRO_STRING_H_
