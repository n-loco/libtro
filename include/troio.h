#ifndef TRO_TROIO_H_
#define TRO_TROIO_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

#include <stddef.h>
#include <stdint.h>
#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif

#include "tro/string.h"

TRO__C_API_BEGIN

#define TRO_BUFFER_CAPACITY 512

typedef struct tro_file tro_file;

typedef int tro_fmodes;

#define TRO_FMODE_READ ((tro_fmodes)1 << 0)
#define TRO_FMODE_WRITE ((tro_fmodes)1 << 1)
#define TRO_FMODE_APPEND ((tro_fmodes)1 << 2)

typedef enum tro_fbufmode {
	TRO_FBUFMODE_NO_BUFFER,
	TRO_FBUFMODE_LINE_BUFFER,
	TRO_FBUFMODE_FULL_BUFFER,
} tro_fbufmode;

TRO__API tro_file *tro_fopen(const char *filepath, tro_fmodes modes);

TRO__API void tro_fclose(tro_file *file);

TRO__API void tro_fsetbuf(tro_file *file, tro_fbufmode mode, size_t capacity);

TRO__API uintptr_t tro_fileno(tro_file *file);

TRO__API bool tro_fis_terminal(tro_file *file);

TRO__API size_t tro_fwrite(tro_file *file, const uint8_t *buffer, size_t bsize);

TRO__API size_t tro_fputc(tro_file *file, tro_urune rune);

TRO__API size_t tro_fputs(tro_file *file, const char *s);

TRO__API bool tro_fflush(tro_file *file);

extern tro_file *troout;
extern tro_file *troin;
extern tro_file *troerr;

TRO__API size_t tro_putc(tro_urune rune);

TRO__API size_t tro_puts(const char *s);

TRO__API size_t tro_eputc(tro_urune rune);

TRO__API size_t tro_eputs(const char *s);

TRO__C_API_END

#endif // TRO_TROIO_H_
