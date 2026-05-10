#include "file.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include "tro/string.h"
#include "tro/dybuffer.h"

#define FILE_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

tro_file *tro_fopen(const char *filepath, tro_fmode mode)
{
	int oflags = O_CLOEXEC | O_CREAT;

	switch (mode) {
	case TRO_FMODE_NULL:
		return NULL;
	case TRO_FMODE_READ:
		oflags |= O_RDONLY;
		break;
	case TRO_FMODE_WRITE:
		oflags |= O_WRONLY;
		break;
	case TRO_FMODE_APPEND:
		oflags |= O_WRONLY | O_APPEND;
		break;
	case TRO_FMODE_RDWT:
		oflags |= O_RDWR;
		break;
	case TRO_FMODE_RDAD:
		oflags |= O_RDWR | O_APPEND;
		break;
	}

	int fd = open(filepath, oflags, FILE_PERMS);
	if (fd < 0)
		return NULL;

	tro_file *file = malloc(sizeof(tro_file));

	*file = (tro_file){
	    .fd = fd,

	    .buffer_mode     = TRO_FBUFMODE_FULL_BUFFER,
	    .buffer_capacity = TRO_BUFFER_CAPACITY,
	    .buffer          = malloc(TRO_BUFFER_CAPACITY),
	    .buffer_size     = 0,

	    .mode     = mode,
	    .terminal = false,
	};

	return file;
}

void tro_fclose(tro_file *file)
{
	tro_fflush(file);
	close(file->fd);
	if (file->buffer != NULL)
		free(file->buffer);
	free(file);
}

void tro_fsetbuf(tro_file *file, tro_fbufmode mode, size_t capacity)
{
	tro_fflush(file);

	const bool no_buffer = mode == TRO_FBUFMODE_NO_BUFFER;

	const bool has_buf_val = file->buffer_capacity != 0 || capacity != 0;
	const bool use_default = !no_buffer && !has_buf_val;

	const bool new_cap = capacity != 0 && file->buffer_capacity != capacity;

	if (no_buffer) {
		if (file->buffer != NULL)
			free(file->buffer);

		file->buffer          = NULL;
		file->buffer_capacity = 0;
	} else if (use_default) {
		if (file->buffer != NULL)
			free(file->buffer);

		file->buffer          = malloc(TRO_BUFFER_CAPACITY);
		file->buffer_capacity = TRO_BUFFER_CAPACITY;
	} else if (new_cap) {
		if (file->buffer != NULL)
			free(file->buffer);

		file->buffer          = malloc(capacity);
		file->buffer_capacity = capacity;
	}

	file->buffer_mode = mode;
	file->buffer_size = 0;
}

uintptr_t tro_fileno(const tro_file *file)
{
	return (uintptr_t)(unsigned int)file->fd;
}

bool tro_fwrites(tro_file *file, const char *data, size_t datal)
{
	if (!writtable(file))
		return false;

	if (datal == 0)
		datal = strlen(data);

	return tro_fwriteb(file, (const uint8_t *)data, datal);
}

bool tro_fwrites16(tro_file *file, const char16_t *data, size_t datal)
{
	if (!writtable(file))
		return false;

	if (datal == 0)
		datal = tro_str16len(data);

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		size_t d8l;
		char *d8 = tro_cnvlloc_str16_to_str(data, datal, &d8l);
		if (d8 == NULL)
			goto WRITE_ERROR;

		ssize_t w = write(file->fd, d8, d8l);

		free(d8);

		if (w < 0)
			goto WRITE_ERROR;

		return true;
	}

	size_t i = 0;
	while (i < datal) {
		const tro_u16code *seq = data + i;
		const size_t seql      = datal - i;

		tro_u8code codes[TRO_MULTI_U8CODE_MAX];
		size_t codesn;
		i += tro_u16codes_to_u8codes(seq, seql, codes, &codesn);

		if (!tro_fwriteb(file, codes, codesn))
			goto WRITE_ERROR;
	}

	return true;

WRITE_ERROR:
	file->buffer_size = 0;
	return false;
}

bool tro_fwriteb(tro_file *file, const uint8_t *data, size_t datal)
{
	if (!writtable(file))
		return false;

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		ssize_t written = write(file->fd, data, datal);
		if (written < 0)
			goto WRITE_ERROR;

		return true;
	}

	for (size_t i = 0; i < datal; i++) {
		file->buffer[file->buffer_size++] = data[i];

		bool buffer_full = file->buffer_size == file->buffer_capacity;
		bool new_line    = false;
		if (file->buffer_mode == TRO_FBUFMODE_LINE_BUFFER)
			new_line = data[i] == '\n';

		if (buffer_full || new_line) {
			bool success = tro_fflush(file);
			if (!success)
				goto WRITE_ERROR;
		}
	}

	return true;

WRITE_ERROR:
	file->buffer_size = 0;
	return false;
}

bool tro_fwritec(tro_file *file, uint32_t c, size_t count)
{
	if (!writtable(file))
		return false;

	tro_u8code codes[TRO_MULTI_U8CODE_MAX];
	size_t codesn = tro_urune_to_u8codes(c, codes);

	for (size_t i = 0; i < count; i++) {
		if (!tro_fwriteb(file, codes, codesn))
			return false;
	}

	return true;
}

tro_dybuf_pref tro_fbufpref(const tro_file *file)
{
	(void)file;
	return TRO_DYBUF_PREF_U8;
}

bool tro_fflush(tro_file *file)
{
	if (!writtable(file))
		return false;

	if (file->buffer_size == 0)
		return true;

	ssize_t w = write(file->fd, file->buffer, file->buffer_size);

	file->buffer_size = 0;
	return w >= 0;
}
