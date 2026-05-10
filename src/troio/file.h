#ifndef FILE_H
#define FILE_H

#include "troio.h"
#include "tro/conditionals.h"

#include <stddef.h>
#include <stdbool.h>

#if TRO_SYSTEM_WIN32
# include <windows.h>
#endif

typedef struct tro_file {
	size_t buffer_capacity;
	size_t buffer_size;
	tro_fbufmode buffer_mode;
	char *buffer;
#if TRO_SYSTEM_WIN32
	WCHAR *wbuffer;
	HANDLE handle;
	bool truncated;
#elif TRO_SYSTEM_UNIX_LIKE
	int fd;
#endif
	tro_fmode mode;
	bool terminal;
} tro_file;

static inline bool readable(const tro_file *file)
{
	return file->mode == TRO_FMODE_READ || file->mode == TRO_FMODE_RDWT ||
	       file->mode == TRO_FMODE_RDAD;
}

static inline bool writtable(const tro_file *file)
{
	return file->mode == TRO_FMODE_WRITE ||
	       file->mode == TRO_FMODE_APPEND || file->mode == TRO_FMODE_RDWT ||
	       file->mode == TRO_FMODE_RDAD;
}

static inline bool appendable(const tro_file *file)
{
	return file->mode == TRO_FMODE_APPEND || file->mode == TRO_FMODE_RDAD;
}

#endif // FILE_H
