#ifndef FILE_H
#define FILE_H

#include "troio.h"
#include "tro/conditionals.h"

#include <stddef.h>
#include <stdbool.h>

#if TRO_SYSTEM_WIN32
# include <windows.h>
#endif

struct tro_file {
	size_t buffer_capacity;
	size_t buffer_size;
	tro_fbufmode buffer_mode;
	char *buffer;
#if TRO_SYSTEM_WIN32
	WCHAR *terminal_buffer;
	HANDLE *handle;
#elif TRO_SYSTEM_UNIX_LIKE
	int fd;
#endif
	tro_fmodes modes;
	bool is_terminal;
};

#define MANDATORY_FMODES (TRO_FMODE_READ | TRO_FMODE_WRITE)

#define FILE_WRITTABLE(file)                                                   \
	(file->modes & TRO_FMODE_WRITE || file->modes & TRO_FMODE_APPEND)

#endif // FILE_H
