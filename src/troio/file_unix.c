#include "file.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>

#include "tro/string.h"

#define FILE_WRITTABLE(file)                                                   \
	(file->modes & TRO_FMODE_WRITE || file->modes & TRO_FMODE_APPEND)

tro_file *tro_fopen(const char *filepath, tro_fmodes modes)
{
	bool has_any_mandatory_mode = modes & MANDATORY_FMODES;
	if (!has_any_mandatory_mode)
		return NULL;

	int oflags = O_CLOEXEC | O_CREAT;

	if ((modes & TRO_FMODE_READ) && (modes & TRO_FMODE_WRITE))
		oflags |= O_RDWR;
	else if (modes & TRO_FMODE_WRITE)
		oflags |= O_WRONLY;
	else if (modes & TRO_FMODE_READ)
		oflags |= O_RDONLY;

	if (modes & TRO_FMODE_APPEND)
		oflags |= O_APPEND;

	mode_t perms =
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	int fd = open(filepath, oflags, perms);

	if (fd < 0)
		return NULL;

	tro_file *file = malloc(sizeof(tro_file));

	file->fd = fd;

	file->buffer_mode     = TRO_FBUFMODE_FULL_BUFFER;
	file->buffer_capacity = TRO_BUFFER_CAPACITY;
	file->buffer          = malloc(TRO_BUFFER_CAPACITY);
	file->buffer_size     = 0;

	file->modes       = modes;
	file->is_terminal = false;

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
	if (mode == TRO_FBUFMODE_NO_BUFFER) {
		if (file->buffer != NULL)
			free(file->buffer);
		file->buffer          = NULL;
		file->buffer_capacity = 0;
	} else {
		if (capacity == 0)
			capacity = TRO_BUFFER_CAPACITY;

		if (file->buffer_capacity != capacity) {
			file->buffer_capacity = capacity;
			if (file->buffer != NULL)
				free(file->buffer);
			file->buffer = malloc(capacity);
		}
	}

	file->buffer_mode = mode;
	file->buffer_size = 0;
}

uintptr_t tro_fileno(tro_file *file)
{
	return (uintptr_t)(unsigned int)file->fd;
}

size_t tro_fwrite(tro_file *file, const uint8_t *buffer, size_t bsize)
{
	if (!FILE_WRITTABLE(file))
		return 0;

	if (!(file->modes & TRO_FMODE_APPEND))
		ftruncate(file->fd, 0);

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		ssize_t written = write(file->fd, buffer, bsize);
		if (written < 0)
			goto WRITE_ERROR;

		return bsize;
	}

	for (size_t i = 0; i < bsize; i++) {
		file->buffer[file->buffer_size] = buffer[i];
		file->buffer_size++;

		bool buffer_full = file->buffer_size == file->buffer_capacity;
		bool new_line    = false;
		if (file->buffer_mode == TRO_FBUFMODE_LINE_BUFFER)
			new_line = buffer[i] == '\n';

		if (buffer_full || new_line) {
			bool success = tro_fflush(file);
			if (!success)
				goto WRITE_ERROR;
		}
	}

	return bsize;

WRITE_ERROR:
	file->buffer_size = 0;
	return 0;
}

size_t tro_fputc(tro_file *file, tro_urune rune)
{
	tro_u8code codes[TRO_MULTI_U8CODE_MAX];
	size_t codes_s = tro_urune_to_u8codes(rune, codes);

	return tro_fwrite(file, codes, codes_s);
}

bool tro_fflush(tro_file *file)
{
	if (!FILE_WRITTABLE(file))
		return false;

	if (file->buffer_size == 0)
		return true;

	ssize_t written = write(file->fd, file->buffer, file->buffer_size);
	if (written < 0) {
		file->buffer_size = 0;
		return false;
	}

	file->buffer_size = 0;
	return true;
}
