#ifndef TRO_TROIO_H_
#define TRO_TROIO_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

#include <stddef.h>
#include <stdint.h>
#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif
#include <stdarg.h>

#include "tro/string.h"
#include "tro/dybuffer.h"

TRO__C_API_BEGIN

#define TRO_BUFFER_CAPACITY 512

typedef struct tro_file tro_file;
TRO__API const tro_dybuffer_i *const tro_fdybuffer_vt;
#define tro_fdybuffer(file)                                                    \
	((tro_dybuffer_obj){                                                   \
	    .obj    = (void *)file,                                            \
	    .vtable = tro_fdybuffer_vt,                                        \
	})

typedef enum tro_fmode {
	TRO_FMODE_NULL,
	TRO_FMODE_READ,
	TRO_FMODE_WRITE,
	TRO_FMODE_APPEND,
	TRO_FMODE_RDWT,
	TRO_FMODE_RDAD,
} tro_fmode;

typedef enum tro_fbufmode {
	TRO_FBUFMODE_NO_BUFFER,
	TRO_FBUFMODE_LINE_BUFFER,
	TRO_FBUFMODE_FULL_BUFFER,
} tro_fbufmode;

TRO__API tro_file *tro_fopen(const char *filepath, tro_fmode mode);

TRO__API void tro_fclose(tro_file *file);

TRO__API void tro_fsetbuf(tro_file *file, tro_fbufmode mode, size_t capacity);

TRO__API uintptr_t tro_fileno(const tro_file *file);

TRO__API bool tro_fterminal(const tro_file *file);

TRO__API bool tro_fwrites(tro_file *file, const char *data, size_t datal);

TRO__API bool tro_fwrites16(tro_file *file, const char16_t *data, size_t datal);

TRO__API bool tro_fwriteb(tro_file *file, const uint8_t *data, size_t datal);

TRO__API bool tro_fwritec(tro_file *file, uint32_t rune, size_t count);

TRO__API tro_dybuf_pref tro_fbufpref(const tro_file *file);

TRO__API bool tro_fputc(tro_file *file, tro_urune rune);

TRO__API bool tro_fputs(tro_file *file, const char *s);

TRO__API bool tro_fputs16(tro_file *file, const char16_t *s);

TRO__API bool tro_fflush(tro_file *file);

TRO__API bool tro_fprintf(tro_file *file, const char *format, ...);

TRO__API bool tro_vfprintf(tro_file *file, const char *format, va_list args);

TRO__API bool tro_fprintf16(tro_file *file, const char16_t *format, ...);

TRO__API bool tro_vfprintf16(tro_file *file, const char16_t *format,
                             va_list args);

extern tro_file *const troout;
extern tro_file *const troin;
extern tro_file *const troerr;

TRO__API bool tro_putc(tro_urune rune);

TRO__API bool tro_puts(const char *s);

TRO__API bool tro_puts16(const char16_t *s);

TRO__API bool tro_eputc(tro_urune rune);

TRO__API bool tro_eputs(const char *s);

TRO__API bool tro_eputs16(const char16_t *s);

TRO__API bool tro_printf(const char *format, ...);

TRO__API bool tro_vprintf(const char *format, va_list args);

TRO__API bool tro_printf16(const char16_t *format, ...);

TRO__API bool tro_vprintf16(const char16_t *format, va_list args);

TRO__API bool tro_eprintf(const char *format, ...);

TRO__API bool tro_veprintf(const char *format, va_list args);

TRO__API bool tro_eprintf16(const char16_t *format, ...);

TRO__API bool tro_veprintf16(const char16_t *format, va_list args);

TRO__C_API_END

#endif // TRO_TROIO_H_
