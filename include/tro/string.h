#ifndef TRO_STRING_H_
#define TRO_STRING_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

#include <stddef.h>
#include <stdint.h>
#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif
#include <stdarg.h>

#include "tro/dybuffer.h"
#include "tro/uchar.h"

TRO__C_API_BEGIN

static inline size_t tro_strulen(const char *str)
{
	return tro_str8_urune_len(str, 0);
}

static inline size_t tro_str16ulen(const tro_char16 *str)
{
	return tro_str16_urune_len(str, 0);
}

TRO__API size_t tro_conv_str_to_str16(const char *in, size_t inlen,
                                      tro_char16 *out, size_t outcap);

TRO__API size_t tro_conv_str16_to_str(const tro_char16 *in, size_t inlen,
                                      char *out, size_t outcap);

TRO__API tro_char16 *tro_cnvlloc_str_to_str16(const char *in, size_t inlen,
                                              size_t *lenout);

TRO__API char *tro_cnvlloc_str16_to_str(const tro_char16 *in, size_t inlen,
                                        size_t *lenout);

TRO__API size_t tro_str16len(const tro_char16 *str);

TRO__API size_t tro_str16nlen(const tro_char16 *str, size_t maxlen);

TRO__API int tro_str16cmp(const tro_char16 *s1, const tro_char16 *s2);

TRO__API int tro_str16ncmp(const tro_char16 *s1, const tro_char16 *s2,
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

TRO__API bool tro_strdybuf_writes16(tro_strdybuf *buf, const tro_char16 *data,
                                    size_t datal);

TRO__API bool tro_strdybuf_writeb(tro_strdybuf *buf, const uint8_t *data,
                                  size_t datal);

TRO__API bool tro_strdybuf_writec(tro_strdybuf *buf, tro_urune rune,
                                  size_t count);

TRO__API tro_dybuf_pref tro_strdybuf_preference(const tro_strdybuf *buf);

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

TRO__API bool tro_str16dybuf_writes16(tro_str16dybuf *buf,
                                      const tro_char16 *data, size_t datal);

TRO__API bool tro_str16dybuf_writeb(tro_str16dybuf *buf, const uint8_t *data,
                                    size_t datal);

TRO__API bool tro_str16dybuf_writec(tro_str16dybuf *buf, uint32_t c32,
                                    size_t count);

TRO__API tro_dybuf_pref tro_str16dybuf_preference(const tro_str16dybuf *buf);

TRO__API size_t tro_str16dybuf_get(tro_str16dybuf *buf, tro_char16 *out,
                                   size_t outcap);

TRO__API tro_char16 *tro_str16dybuf_getlloc(tro_str16dybuf *buf,
                                            size_t *outlen);

TRO__API bool tro_sfmt(tro_dybuffer_obj buf, const char *format, ...);

TRO__API bool tro_vsfmt(tro_dybuffer_obj buf, const char *format, va_list args);

TRO__API bool tro_sfmt16(tro_dybuffer_obj buf, const tro_char16 *format, ...);

TRO__API bool tro_vsfmt16(tro_dybuffer_obj buf, const tro_char16 *format,
                          va_list args);

TRO__API char *tro_str_fmt(size_t *outlen, const char *format, ...);

TRO__API char *tro_vstr_fmt(size_t *outlen, const char *format, va_list args);

TRO__API char *tro_str_fmt16(size_t *outlen, const tro_char16 *format, ...);

TRO__API char *tro_vstr_fmt16(size_t *outlen, const tro_char16 *format,
                              va_list args);

TRO__API tro_char16 *tro_str16_fmt(size_t *outlen, const char *format, ...);

TRO__API tro_char16 *tro_vstr16_fmt(size_t *outlen, const char *format,
                                    va_list args);

TRO__API tro_char16 *tro_str16_fmt16(size_t *outlen, const tro_char16 *format,
                                     ...);

TRO__API tro_char16 *tro_vstr16_fmt16(size_t *outlen, const tro_char16 *format,
                                      va_list args);

TRO__C_API_END

#endif // TRO_STRING_H_
