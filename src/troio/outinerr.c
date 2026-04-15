#include "troio.h"
#include "file.h"
#include "tro/conditionals.h"

#include <stddef.h>
#include <stdlib.h>

#if TRO_SYSTEM_WIN32
# include <windows.h>
#elif TRO_SYSTEM_UNIX_LIKE
# include <unistd.h>
#endif

static struct tro_file outfstream;
static struct tro_file infstream;
static struct tro_file errfstream;

tro_file *troout = &outfstream;
tro_file *troin  = &infstream;
tro_file *troerr = &errfstream;

size_t tro_putc(tro_urune rune)
{
	return tro_fputc(troout, rune);
}

size_t tro_puts(const char *s)
{
	return tro_fputs(troout, s);
}

size_t tro_eputc(tro_urune rune)
{
	return tro_fputc(troerr, rune);
}

size_t tro_eputs(const char *s)
{
	return tro_fputs(troerr, s);
}

// Inicializadores e de-inicializadores abaixo.

#if TRO_SYSTEM_WIN32
static DWORD og_out_mode;
static DWORD og_in_mode;
static DWORD og_err_mode;
#endif

#if TRO_SYSTEM_WIN32
// Definição de construtor abaixo da função.
#elif TRO_SYSTEM_UNIX_LIKE && defined(__GNUC__)
__attribute__((constructor))
#else
# error "Unnable to specify constructor."
#endif
static void outinerr_ctor(void)
{
#if TRO_SYSTEM_WIN32
	HANDLE out_handle    = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL out_is_terminal = GetConsoleMode(out_handle, &og_out_mode);

	HANDLE in_handle    = GetStdHandle(STD_INPUT_HANDLE);
	BOOL in_is_terminal = GetConsoleMode(in_handle, &og_in_mode);

	HANDLE err_handle    = GetStdHandle(STD_ERROR_HANDLE);
	BOOL err_is_terminal = GetConsoleMode(err_handle, &og_err_mode);

	troout->handle = out_handle;
	troin->handle  = in_handle;
	troerr->handle = err_handle;

	troout->is_terminal = out_is_terminal;
	troin->is_terminal  = in_is_terminal;
	troerr->is_terminal = err_is_terminal;

	DWORD new_out_mode = og_out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	DWORD new_in_mode  = og_in_mode | ENABLE_VIRTUAL_TERMINAL_INPUT;
	DWORD new_err_mode = og_err_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	if (out_is_terminal)
		SetConsoleMode(out_handle, new_out_mode);
	if (in_is_terminal)
		SetConsoleMode(in_handle, new_in_mode);
	if (err_is_terminal)
		SetConsoleMode(err_handle, new_err_mode);

	if (out_is_terminal)
		troout->wbuffer = malloc(TRO_BUFFER_CAPACITY * sizeof(WCHAR));
	else
		troout->buffer = malloc(TRO_BUFFER_CAPACITY);

	if (err_is_terminal)
		troerr->wbuffer = malloc(TRO_BUFFER_CAPACITY * sizeof(WCHAR));
	else
		troerr->buffer = malloc(TRO_BUFFER_CAPACITY);
#elif TRO_SYSTEM_UNIX_LIKE
	troout->fd = STDOUT_FILENO;
	troin->fd  = STDIN_FILENO;
	troerr->fd = STDERR_FILENO;

	troout->is_terminal = isatty(STDOUT_FILENO);
	troin->is_terminal  = isatty(STDIN_FILENO);
	troerr->is_terminal = isatty(STDERR_FILENO);

	troout->buffer = malloc(TRO_BUFFER_CAPACITY);
	troerr->buffer = malloc(TRO_BUFFER_CAPACITY);
#endif
}

// clang-format off
#if TRO_SYSTEM_WIN32

#pragma section(".CRT$XCU", read)
__declspec(allocate(".CRT$XCU"))
void (*tro__outinerr_ctor)(void) = outinerr_ctor;

#endif
// clang-format on

#if TRO_SYSTEM_WIN32
// Definição de destruidor abaixo da função.
#elif TRO_SYSTEM_UNIX_LIKE && defined(__GNUC__)
__attribute__((destructor))
#else
# error "Unnable to specify destructor."
#endif
static void outinerr_dtor(void)
{
	tro_fflush(troout);
	tro_fflush(troerr);

	if (troout->buffer != NULL)
		free(troout->buffer);
	if (troin->buffer != NULL)
		free(troin->buffer);
	if (troerr->buffer != NULL)
		free(troerr->buffer);

#if TRO_SYSTEM_WIN32
	if (troout->is_terminal)
		SetConsoleMode(troout->handle, og_out_mode);
	if (troin->is_terminal)
		SetConsoleMode(troin->handle, og_in_mode);
	if (troerr->is_terminal)
		SetConsoleMode(troerr->handle, og_err_mode);

	if (troout->wbuffer != NULL)
		free(troout->wbuffer);
	if (troin->wbuffer != NULL)
		free(troin->wbuffer);
	if (troerr->wbuffer != NULL)
		free(troerr->wbuffer);
#endif
}

// clang-format off
#if TRO_SYSTEM_WIN32

#pragma section(".CRT$XPU", read)
__declspec(allocate(".CRT$XPU"))
void (*tro__outinerr_dtor)(void) = outinerr_dtor;

#endif
// clang-format on

static struct tro_file outfstream = {
    .buffer_capacity = TRO_BUFFER_CAPACITY,
    .modes           = TRO_FMODE_WRITE,
    .buffer_mode     = TRO_FBUFMODE_LINE_BUFFER,
};
static struct tro_file infstream = {
    .modes       = TRO_FMODE_READ,
    .buffer_mode = TRO_FBUFMODE_NO_BUFFER,
};
static struct tro_file errfstream = {
    .buffer_capacity = TRO_BUFFER_CAPACITY,
    .modes           = TRO_FMODE_WRITE,
    .buffer_mode     = TRO_FBUFMODE_LINE_BUFFER,
};
