#include "file.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <windows.h>

#include "tro/string.h"
#include "tro/uchar.h"
#include "tro/dybuffer.h"

tro_file *tro_fopen(const char *filepath, tro_fmode mode)
{
	DWORD desired_access = 0;

	switch (mode) {
	case TRO_FMODE_NULL:
		return NULL;
	case TRO_FMODE_READ:
		desired_access |= FILE_READ_DATA;
		break;
	case TRO_FMODE_WRITE:
		desired_access |= FILE_WRITE_DATA;
		break;
	case TRO_FMODE_APPEND:
		desired_access |= FILE_APPEND_DATA;
		break;
	case TRO_FMODE_RDWT:
		desired_access |= FILE_READ_DATA | FILE_WRITE_DATA;
		break;
	case TRO_FMODE_RDAD:
		desired_access |= FILE_READ_DATA | FILE_APPEND_DATA;
		break;
	}

	HANDLE handle;
	{
		WCHAR *wfilepath = tro_cnvlloc_str_to_str16(filepath, 0, NULL);
		if (wfilepath == NULL)
			return NULL;

		handle = CreateFileW(wfilepath, desired_access, 0, NULL,
		                     OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		free(wfilepath);
	}
	if (handle == INVALID_HANDLE_VALUE)
		return NULL;

	if (GetLastError() == ERROR_ALREADY_EXISTS)
		SetLastError(ERROR_SUCCESS);
	else
		return NULL;

	tro_file *file = malloc(sizeof(tro_file));

	*file = (tro_file){
	    .handle = handle,

	    .buffer_capacity = TRO_BUFFER_CAPACITY,
	    .buffer_mode     = TRO_FBUFMODE_NO_BUFFER,
	    .buffer          = malloc(TRO_BUFFER_CAPACITY),
	    .wbuffer         = NULL,
	    .buffer_size     = 0,

	    .truncated = false,
	    .mode      = mode,
	    .terminal  = false,
	};

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
	tro_fflush(file);

	const bool no_buffer = mode == TRO_FBUFMODE_NO_BUFFER;

	const bool has_buf_val = file->buffer_capacity != 0 || capacity != 0;
	const bool use_default = !no_buffer && !has_buf_val;

	const bool new_cap = capacity != 0 && file->buffer_capacity != capacity;

	if (no_buffer) {
		if (file->buffer != NULL)
			free(file->buffer);
		if (file->wbuffer != NULL)
			free(file->wbuffer);

		file->buffer          = NULL;
		file->wbuffer         = NULL;
		file->buffer_capacity = 0;
	} else if (use_default) {
		if (file->buffer != NULL)
			free(file->buffer);
		if (file->wbuffer != NULL)
			free(file->wbuffer);

		if (file->terminal)
			file->wbuffer =
			    malloc(TRO_BUFFER_CAPACITY * sizeof(WCHAR));
		else
			file->buffer = malloc(TRO_BUFFER_CAPACITY);
		file->buffer_capacity = TRO_BUFFER_CAPACITY;
	} else if (new_cap) {
		if (file->buffer != NULL)
			free(file->buffer);
		if (file->wbuffer != NULL)
			free(file->wbuffer);

		if (file->terminal)
			file->wbuffer = malloc(capacity * sizeof(WCHAR));
		else
			file->buffer = malloc(capacity);
		file->buffer_capacity = capacity;
	}

	file->buffer_mode = mode;
	file->buffer_size = 0;
}

uintptr_t tro_fileno(const tro_file *file)
{
	return (uintptr_t)file->handle;
}

static bool fwrites_file(tro_file *file, const char *data, size_t datal);
static bool fwrites_term(tro_file *file, const char *data, size_t datal);

bool tro_fwrites(tro_file *file, const char *data, size_t datal)
{
	if (file->terminal)
		return fwrites_term(file, data, datal);
	return fwrites_file(file, data, datal);
}

static bool fwrites16_file(tro_file *file, const tro_char16 *data,
                           size_t datal);
static bool fwrites16_term(tro_file *file, const tro_char16 *data,
                           size_t datal);

bool tro_fwrites16(tro_file *file, const tro_char16 *data, size_t datal)
{
	if (file->terminal)
		return fwrites16_term(file, data, datal);
	return fwrites16_file(file, data, datal);
}

bool tro_fwriteb(tro_file *file, const uint8_t *data, size_t datal)
{
	if (file->terminal)
		return fwrites_term(file, (const char *)data, datal);
	return fwrites_file(file, (const char *)data, datal);
}

static bool fwritec_file(tro_file *file, uint32_t rune, size_t count);
static bool fwritec_term(tro_file *file, uint32_t rune, size_t count);

bool tro_fwritec(tro_file *file, tro_char16 rune, size_t count)
{
	if (file->terminal)
		return fwritec_term(file, rune, count);
	return fwritec_file(file, rune, count);
}

tro_dybuf_pref tro_fbufpref(const tro_file *file)
{
	if (file->terminal)
		return TRO_DYBUF_PREF_U16;
	return TRO_DYBUF_PREF_U8;
}

bool tro_fflush(tro_file *file)
{
	if (!writtable(file))
		return false;

	if (file->buffer_size == 0)
		return true;

	BOOL ok;
	if (file->terminal)
		ok = WriteConsoleW(file->handle, file->wbuffer,
		                   (DWORD)file->buffer_size, NULL, NULL);
	else
		ok = WriteFile(file->handle, file->buffer,
		               (DWORD)file->buffer_size, NULL, NULL);

	file->buffer_size = 0;
	return (bool)ok;
}

static bool truncate(tro_file *file);

static bool fwrites_file(tro_file *file, const char *data, size_t datal)
{
	if (!writtable(file))
		return false;

	if (!truncate(file))
		return false;

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		if (!WriteFile(file->handle, data, (DWORD)datal, NULL, NULL))
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

static bool fwrites16_file(tro_file *file, const tro_char16 *data, size_t datal)
{
	if (!writtable(file))
		return false;

	if (!truncate(file))
		return false;

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		size_t datal8;
		char *data8 = tro_cnvlloc_str16_to_str(data, datal, &datal8);

		bool success = fwrites_file(file, data8, datal8);

		free(data8);
		return success;
	}

	return true;

	size_t i = 0;
	while (i < datal) {
		bool new_line = data[i] == '\n' &&
		                file->buffer_mode == TRO_FBUFMODE_LINE_BUFFER;

		if (new_line) {
			i++;
			file->buffer[file->buffer_size++] = '\n';
			if (!tro_fflush(file))
				goto WRITE_ERROR;
			continue;
		}

		const tro_u16code *seq = (data + i);
		const size_t seql      = datal - i;

		tro_u8code codes[TRO_MULTI_U8CODE_MAX];
		size_t codesn;
		i += tro_u16codes_to_u8codes(seq, seql, codes, &codesn);

		for (size_t j = 0; j < codesn; j++) {
			file->buffer[file->buffer_size++] = codes[j];

			if (file->buffer_size == file->buffer_capacity) {
				bool success = tro_fflush(file);
				if (!success)
					goto WRITE_ERROR;
			}
		}
	}

WRITE_ERROR:
	file->buffer_size = 0;
	return false;
}

static bool fwritec_file(tro_file *file, tro_urune rune, size_t count)
{
	if (!writtable(file))
		return false;

	tro_u8code codes[TRO_MULTI_U8CODE_MAX];
	size_t codesn = tro_urune_to_u8codes(rune, codes);

	for (size_t i = 0; i < count; i++) {
		if (!fwrites_file(file, (const char *)codes, codesn))
			return false;
	}

	return true;
}

static bool fwrites_term(tro_file *file, const char *data, size_t datal)
{
	if (!writtable(file))
		return false;

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		size_t data16l;
		tro_char16 *data16 =
		    tro_cnvlloc_str_to_str16(data, datal, &data16l);

		bool success = fwrites16_term(file, data16, data16l);

		free(data16);
		return success;
	}

	size_t i = 0;
	while (i < datal) {
		bool new_line = data[i] == '\n' &&
		                file->buffer_mode == TRO_FBUFMODE_LINE_BUFFER;

		if (new_line) {
			i++;
			file->wbuffer[file->buffer_size++] = u'\n';
			if (!tro_fflush(file))
				goto WRITE_ERROR;
			continue;
		}

		const tro_u8code *seq = (const tro_u8code *)(data + i);
		const size_t seql     = datal - i;

		tro_u16code codes[TRO_MULTI_U16CODE_MAX];
		size_t codesn;
		i += tro_u8codes_to_u16codes(seq, seql, codes, &codesn);

		for (size_t i = 0; i < codesn; i++) {
			file->wbuffer[file->buffer_size++] = codes[i];

			if (file->buffer_size == file->buffer_capacity) {
				bool success = tro_fflush(file);
				if (!success)
					goto WRITE_ERROR;
			}
		}
	}

	return true;

WRITE_ERROR:
	file->buffer_size = 0;
	return false;
}

static bool fwrites16_term(tro_file *file, const WCHAR *data, size_t datal)
{
	if (!writtable(file))
		return false;

	if (file->buffer_mode == TRO_FBUFMODE_NO_BUFFER) {
		BOOL ok =
		    WriteConsoleW(file->handle, data, (DWORD)datal, NULL, NULL);
		if (!ok)
			goto WRITE_ERROR;

		return true;
	}

	for (size_t i = 0; i < datal; i++) {
		file->wbuffer[file->buffer_size++] = data[i];

		bool buffer_full = file->buffer_size == file->buffer_capacity;
		bool new_line    = false;
		if (file->buffer_mode == TRO_FBUFMODE_LINE_BUFFER)
			new_line = data[i] == u'\n';

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

static bool fwritec_term(tro_file *file, tro_urune rune, size_t count)
{
	if (!writtable(file))
		return false;

	tro_u16code codes[TRO_MULTI_U16CODE_MAX];
	size_t codesn = tro_urune_to_u16codes(rune, codes);

	for (size_t i = 0; i < count; i++) {
		if (!fwrites16_term(file, codes, codesn))
			return false;
	}

	return true;
}

static bool truncate(tro_file *file)
{
	if (!writtable(file))
		return false;

	if (file->truncated || appendable(file))
		return true;

	const LARGE_INTEGER zero = {0};
	if (!SetFilePointerEx(file->handle, zero, NULL, FILE_BEGIN))
		return false;

	if (!SetEndOfFile(file->handle))
		return false;

	file->truncated = true;
	return true;
}
