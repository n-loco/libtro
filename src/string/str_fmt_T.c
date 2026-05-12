#include "tro/string.h"

#include <stddef.h>
#include <stdarg.h>

#include "tro/uchar.h"

#ifdef USE_CHAR16_T
# define CHAR_T tro_char16

# define tro_vstr_fmt_T tro_vstr16_fmt
# define tro_str_fmt_T tro_str16_fmt
# define tro_vstr_fmt16_T tro_vstr16_fmt16
# define tro_str_fmt16_T tro_str16_fmt16

# define tro_strdybuf_T tro_str16dybuf
# define tro_create_strdybuf_T tro_create_str16dybuf
# define tro_destroy_strdybuf_T tro_destroy_str16dybuf
# define tro_strdybuf_getlloc_T tro_str16dybuf_getlloc
# define tro_strdybuf_dybuffer_T(buf) tro_str16dybuf_dybuffer(buf)
#else
# define CHAR_T char

# define tro_vstr_fmt_T tro_vstr_fmt
# define tro_str_fmt_T tro_str_fmt
# define tro_vstr_fmt16_T tro_vstr_fmt16
# define tro_str_fmt16_T tro_str_fmt16

# define tro_strdybuf_T tro_strdybuf
# define tro_create_strdybuf_T tro_create_strdybuf
# define tro_destroy_strdybuf_T tro_destroy_strdybuf
# define tro_strdybuf_getlloc_T tro_strdybuf_getlloc
# define tro_strdybuf_dybuffer_T(buf) tro_strdybuf_dybuffer(buf)
#endif

CHAR_T *tro_str_fmt_T(size_t *outlen, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	CHAR_T *str = tro_vstr_fmt_T(outlen, format, args);
	va_end(args);
	return str;
}

CHAR_T *tro_vstr_fmt_T(size_t *outlen, const char *format, va_list args)
{
	tro_strdybuf_T *buf = tro_create_strdybuf_T(256);

	CHAR_T *str = NULL;
	if (tro_vsfmt(tro_strdybuf_dybuffer_T(buf), format, args))
		str = tro_strdybuf_getlloc_T(buf, outlen);
	else if (outlen != NULL)
		*outlen = 0;

	tro_destroy_strdybuf_T(buf);
	return str;
}

CHAR_T *tro_str_fmt16_T(size_t *outlen, const tro_char16 *format, ...)
{
	va_list args;
	va_start(args, format);
	CHAR_T *str = tro_vstr_fmt16_T(outlen, format, args);
	va_end(args);
	return str;
}

CHAR_T *tro_vstr_fmt16_T(size_t *outlen, const tro_char16 *format, va_list args)
{
	tro_strdybuf_T *buf = tro_create_strdybuf_T(256);

	CHAR_T *str = NULL;
	if (tro_vsfmt16(tro_strdybuf_dybuffer_T(buf), format, args))
		str = tro_strdybuf_getlloc_T(buf, outlen);
	else if (outlen != NULL)
		*outlen = 0;

	tro_destroy_strdybuf_T(buf);
	return str;
}
