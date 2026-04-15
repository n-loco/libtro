#include "file.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <windows.h>

#include "tro/string.h"

tro_file *tro_fopen(const char *filepath, tro_fmodes modes)
{
	bool has_any_mandatory_mode = modes & MANDATORY_FMODES;
	if (!has_any_mandatory_mode)
		return NULL;

	DWORD desired_access = 0;

	if (modes & TRO_FMODE_READ)
		desired_access |= FILE_READ_DATA;
	
	if ((modes & TRO_FMODE_WRITE) && (modes & TRO_FMODE_APPEND))
		desired_access |= FILE_APPEND_DATA;
	else if (modes & TRO_FMODE_WRITE)
		desired_access |= FILE_WRITE_DATA;

	HANDLE handle = NULL;
	{
		WCHAR *wfilepath = tro_cnvlloc_str_to_str16(filepath, 0, NULL);

		handle = CreateFileW(wfilepath, desired_access, 0, NULL,
		                     OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		free(wfilepath);
	}
	if (handle == NULL)
		return NULL;

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		SetLastError(ERROR_SUCCESS);
	else
		return NULL;

	tro_file *file = malloc(sizeof(tro_file));

	file->handle = handle;

	file->buffer_capacity = TRO_BUFFER_CAPACITY;
	file->buffer_size     = 0;
	file->buffer_mode     = modes;
	file->buffer          = malloc(TRO_BUFFER_CAPACITY);
	file->wbuffer         = NULL;

	file->modes       = modes;
	file->is_terminal = false;

	return file;
}

void tro_fclose(tro_file *file)
{
	tro_fflush(file);
	CloseHandle(file->handle);
	if (file->buffer != NULL)
		free(file->buffer);
	if (file->wbuffer != NULL)
		free(file->wbuffer);
	free(file);
}

void tro_fsetbuf(tro_file *file, tro_fbufmode mode, size_t capacity)
{
	if (mode == TRO_FBUFMODE_NO_BUFFER) {
		if (file->buffer != NULL)
			free(file->buffer);
		if (file->wbuffer != NULL)
			free(file->wbuffer);
		file->buffer          = NULL;
		file->wbuffer         = NULL;
		file->buffer_capacity = 0;
	} else {
		if (capacity == 0)
			capacity = TRO_BUFFER_CAPACITY;

		if (file->buffer_capacity != capacity) {
			file->buffer_capacity = capacity;

			if (file->buffer != NULL)
				free(file->buffer);
			if (file->wbuffer != NULL)
				free(file->wbuffer);

			if (file->is_terminal)
				file->wbuffer =
				    malloc(capacity * sizeof(WCHAR));
			else
				file->buffer = malloc(capacity);
		}
	}

	file->buffer_mode = mode;
	file->buffer_size = 0;
}

uintptr_t tro_fileno(tro_file *file)
{
	return (uintptr_t)file->handle;
}

static size_t fwrite_file(tro_file *file, const uint8_t *buffer, size_t bsize);
static size_t fwrite_term(tro_file *file, const uint8_t *buffer, size_t bsize);

size_t tro_fwrite(tro_file *file, const uint8_t *buffer, size_t bsize)
{
	if (file->is_terminal)
		return fwrite_term(file, buffer, bsize);
	return fwrite_file(file, buffer, bsize);
}

static size_t fwrite_file(tro_file *file, const uint8_t *buffer, size_t bsize)
{
	if (!FILE_WRITTABLE(file))
		return 0;

	if (!(file->modes & TRO_FMODE_APPEND)) {
		LARGE_INTEGER zero = {0};
		SetFilePointerEx(file->handle, zero, NULL, FILE_BEGIN);
		SetEndOfFile(file);
	}

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		BOOL ok =
		    WriteFile(file->handle, buffer, (DWORD)bsize, NULL, NULL);
		if (!ok)
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

static size_t wfwrite_term(tro_file *file, const WCHAR *buffer, size_t bsize);

static size_t fwrite_term(tro_file *file, const uint8_t *buffer, size_t bsize)
{
	if (!FILE_WRITTABLE(file))
		return 0;

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		size_t wbsize;
		WCHAR *wbuffer = tro_cnvlloc_str_to_str16((const char *)buffer,
		                                          bsize, &wbsize);

		size_t written = wfwrite_term(file, wbuffer, wbsize);

		free(wbuffer);
		return written > 0 ? bsize : 0;
	}

	size_t i = 0;
	while (i < bsize) {
		bool new_line = buffer[i] == '\n' &&
		                file->buffer_mode == TRO_FBUFMODE_LINE_BUFFER;

		if (new_line) {
			i++;
			file->wbuffer[file->buffer_size] = u'\n';
			file->buffer_size++;
			bool success = tro_fflush(file);
			if (!success)
				goto WRITE_ERROR;
			continue;
		}

		const tro_u8code *seq = buffer + i;
		tro_urune rune;
		i += tro_u8codes_to_urune(seq, &rune);
		tro_u16code surrogates[TRO_MULTI_U16CODE_MAX];
		size_t surrogates_n = tro_urune_to_u16codes(rune, surrogates);

		for (size_t i = 0; i < surrogates_n; i++) {
			file->wbuffer[file->buffer_size] = surrogates[i];
			file->buffer_size++;

			if (file->buffer_size == file->buffer_capacity) {
				bool success = tro_fflush(file);
				if (!success)
					goto WRITE_ERROR;
			}
		}
	}

	return bsize;

WRITE_ERROR:
	file->buffer_size = 0;
	return 0;
}

static size_t wfwrite_term(tro_file *file, const WCHAR *buffer, size_t bsize)
{
	if (!FILE_WRITTABLE(file))
		return 0;

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		BOOL ok = WriteConsoleW(file->handle, buffer, (DWORD)bsize,
		                        NULL, NULL);
		if (!ok)
			goto WRITE_ERROR;

		return bsize;
	}

	for (size_t i = 0; i < bsize; i++) {
		file->wbuffer[file->buffer_size] = buffer[i];
		file->buffer_size++;

		bool buffer_full = file->buffer_size == file->buffer_capacity;
		bool new_line    = false;
		if (file->buffer_mode == TRO_FBUFMODE_LINE_BUFFER)
			new_line = buffer[i] == u'\n';

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
	if (file->is_terminal) {
		tro_u16code surrogates[TRO_MULTI_U16CODE_MAX];
		size_t surrogates_n = tro_urune_to_u16codes(rune, surrogates);
		return wfwrite_term(file, surrogates, surrogates_n);
	}

	tro_u8code bytes[TRO_MULTI_U8CODE_MAX];
	size_t bytes_n = tro_urune_to_u8codes(rune, bytes);
	return fwrite_file(file, bytes, bytes_n);
}

bool tro_fflush(tro_file *file)
{
	if (!FILE_WRITTABLE(file))
		return false;

	if (file->buffer_size == 0)
		return true;

	BOOL ok;
	if (file->is_terminal)
		ok = WriteConsoleW(file->handle, file->wbuffer,
		                   (DWORD)file->buffer_size, NULL, NULL);
	else
		ok = WriteFile(file->handle, file->buffer,
		               (DWORD)file->buffer_size, NULL, NULL);

	file->buffer_size = 0;
	return ok;
}
