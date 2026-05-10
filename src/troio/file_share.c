#include "file.h"

#include <stdbool.h>
#include <stdint.h>
#include <uchar.h>
#include <stdarg.h>

#include "tro/dybuffer.h"
#include "tro/string.h"

static const tro_dybuffer_i vt = {
    .writes     = (bool (*)(void *, const char *, size_t))tro_fwrites,
    .writes16   = (bool (*)(void *, const char16_t *, size_t))tro_fwrites16,
    .writeb     = (bool (*)(void *, const uint8_t *, size_t))tro_fwriteb,
    .writec     = (bool (*)(void *, uint32_t, size_t))tro_fwritec,
    .preference = (tro_dybuf_pref(*)(const void *))tro_fbufpref,
};

const tro_dybuffer_i *const tro_fdybuffer_vt = &vt;

bool tro_fterminal(const tro_file *file)
{
	return file->terminal;
}

bool tro_fputc(tro_file *file, tro_urune rune)
{
	return tro_fwritec(file, rune, 1);
}

bool tro_fputs(tro_file *file, const char *s)
{
	return tro_fwrites(file, s, 0);
}

bool tro_fputs16(tro_file *file, const char16_t *s)
{
	return tro_fwrites16(file, s, 0);
}

bool tro_fprintf(tro_file *file, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	bool r = tro_vfprintf(file, format, args);
	va_end(args);
	return r;
}

bool tro_vfprintf(tro_file *file, const char *format, va_list args)
{
	return tro_vsfmt(tro_fdybuffer(file), format, args);
}

bool tro_fprintf16(tro_file *file, const char16_t *format, ...)
{
	va_list args;
	va_start(args, format);
	bool r = tro_vfprintf16(file, format, args);
	va_end(args);
	return r;
}

bool tro_vfprintf16(tro_file *file, const char16_t *format, va_list args)
{
	return tro_vsfmt16(tro_fdybuffer(file), format, args);
}
