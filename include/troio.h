/** @file */

#ifndef TRO_TROIO_H_
#define TRO_TROIO_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"
#include "tro/private/doxy.h"

#include <stddef.h>
#include <stdint.h>
#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif
#include <stdarg.h>

#include "tro/dybuffer.h"
#include "tro/uchar.h"

TRO__C_API_BEGIN

/**
 * @def TRO_BUFFER_CAPACITY
 *
 * Capacidade padrão do *buffer* de @ref tro_file.
 *
 * @see tro_fsetbuf
 */
#define TRO_BUFFER_CAPACITY 512

/**
 * Normalmente, representa um **arquivo**
 * do sistema, mas também pode representar
 * um **console/terminal**.
 * Implementa a **interface** `tro_dybuffer`.
 *
 * @see tro_fdybuffer
 * @see troout;
 * @see troin;
 * @see troerr;
 */
typedef struct tro_file tro_file;
/**
 * **Tabela virtual** com os métodos de @ref tro_file
 * para a **interface** `tro_dybuffer`.
 */
TRO__API const tro_dybuffer_i *const tro_fdybuffer_vt;
/**
 * @def tro_fdybuffer(file)
 *
 * Macro de **cast** de @ref tro_file para @ref tro_dybuffer_obj.
 */
#define tro_fdybuffer(file)                                                    \
	((tro_dybuffer_obj){                                                   \
	    .obj    = (void *)file,                                            \
	    .vtable = tro_fdybuffer_vt,                                        \
	})

/**
 * Representa o **modo de acesso**
 * de um @ref tro_file.
 */
typedef enum tro__ndoxy(tro_fmode) {
	/**
	 * *Nenhum modo*. Só aparece
	 * quando há problemas.
	 * **NÃO** deve ser usado para
	 * *abrir* um @ref tro_file.
	 */
	TRO_FMODE_NULL,
	/**
	 * @ref tro_file é **apenas leitura**.
	 */
	TRO_FMODE_READ,
	/**
	 * @ref tro_file é **apenas escrita**.
	 */
	TRO_FMODE_WRITE,
	/**
	 * @ref tro_file é **apenas escrita aditiva**.
	 */
	TRO_FMODE_APPEND,
	/**
	 * @ref tro_file é **leitura e escrita**.
	 */
	TRO_FMODE_RDWT,
	/**
	 * @ref tro_file é **leitura e escrita aditiva**.
	 */
	TRO_FMODE_RDAD,
} tro_fmode;

/**
 * Representa o **modo de gerenciamento**
 * do *buffer interno* de @ref tro_file.
 *
 * @see tro_fsetbuf
 */
typedef enum tro__ndoxy(tro_fbufmode) {
	/**
	 * @ref tro_file simplesmente
	 * **não** usa um *buffer*, escreve
	 * tudo **imediatamente**.
	 */
	TRO_FBUFMODE_NO_BUFFER,
	/**
	 * @ref tro_file fica armazenando os dados
	 * escritos em seu *buffer interno* até um `'\n'`
	 * ser escrito (ou até o *buffer* ficar cheio)
	 * para aí sim escrever os dados do *buffer interno*
	 * no arquivo real.
	 */
	TRO_FBUFMODE_LINE_BUFFER,
	/**
	 * @ref tro_file fica armazenando os dados
	 * escritos em seu *buffer interno* até
	 * ele ficar cheio para aí sim escrever
	 * os dados do *buffer interno*
	 * no arquivo real.
	 */
	TRO_FBUFMODE_FULL_BUFFER,
} tro_fbufmode;

/**
 * Abre um arquivo ou cria um se não existir.
 *
 * Por padrão o **modo de gerenciamento** do
 * *buffer interno* é @ref TRO_FBUFMODE_FULL_BUFFER.
 *
 * @param filepath é o caminho do arquivo.
 * @param mode é o **modo de acesso**.
 *
 * @returns um **ponteiro** para o objeto
 * @ref tro_file aberto, `NULL` em caso de falha.
 *
 * @see tro_fclose
 */
TRO__API tro_file *tro_fopen(const char *filepath, tro_fmode mode);

/**
 * Fecha um arquivo e automaticamente
 * usa @ref tro_fflush.
 *
 * @param file o arquivo a ser fechado.
 */
TRO__API void tro_fclose(tro_file *file);

/**
 * Configura o *buffer interno* de um @ref tro_file.
 *
 * @param file o objeto.
 * @param mode o **modo de gerenciamento** do *buffer interno*.
 * @param capacity é a capacidade do *buffer interno*. Se
 * for `0`, não muda a capacidade. Completamente ignorado se
 * `mode` for @ref TRO_FBUFMODE_NO_BUFFER.
 */
TRO__API void tro_fsetbuf(tro_file *file, tro_fbufmode mode, size_t capacity);

/**
 * @param file o objeto.
 *
 * @returns o **identificador nativo** do
 * arquivo aberto, sendo um `HANDLE` em **Win32**
 * ou um `int` (*file descriptor*) em **ambiente
 * tipo Unix**, então cast é seguro.
 * ```c
 * #if TRO_SYSTEM_WIN32
 * HANDLE hnd = (HANDLE)tro_fileno(file);
 * // Código Windows...
 * #elif TRO_SYSTEM_UNIX_LIKE
 * int fd = (int)tro_fileno(file);
 * // Código POSIX...
 * #else
 * # error "..."
 * #endif
 * ```
 *
 * @see tro/conditionals.h
 */
TRO__API uintptr_t tro_fileno(const tro_file *file);

/**
 * @param file o objeto.
 *
 * @returns `true` se `file` for um
 * **console/terminal**, `false`
 * caso contrário.
 */
TRO__API bool tro_fterminal(const tro_file *file);

/**
 * @param file o objeto.
 * @param data é uma **string UTF-8**.
 * @param datal é o comprimento de `data`,
 * caso seja `0`, se assume `data` terminado
 * em `'\0'`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fwrites(tro_file *file, const char *data, size_t datal);

/**
 * @param file o objeto.
 * @param data é uma **string UTF-16**.
 * @param datal é o comprimento de `data`,
 * caso seja `0`, se assume `data` terminado
 * em `'\0'`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fwrites16(tro_file *file, const tro_char16 *data,
                            size_t datal);

/**
 * @param file o objeto.
 * @param data é uma **sequência de bytes arbitrária**.
 * @param datal é o comprimento de `data`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fwriteb(tro_file *file, const uint8_t *data, size_t datal);

/**
 * @param file o objeto.
 * @param rune é uma **runa** (**code point**).
 * @param count diz quantas vezes se deve escrever `rune`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fwritec(tro_file *file, uint32_t rune, size_t count);

/**
 * @param file o objeto.
 *
 * @returns a *preferência de encoding*.
 */
TRO__API tro_dybuf_pref tro_fbufpref(const tro_file *file);

/**
 * Escreve uma **runa** em um @ref tro_file.
 *
 * @param file o objeto.
 * @param rune é a **runa** (**code point**).
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fputc(tro_file *file, tro_urune rune);

/**
 * Escreve uma *string* terminada
 * em `'\0'` em um @ref tro_file.
 *
 * @param file o objeto.
 * @param s é **string UTF-8**.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fputs(tro_file *file, const char *s);

/**
 * Escreve uma *string* terminada
 * em `'\0'` em um @ref tro_file.
 *
 * @param file o objeto.
 * @param s é **string UTF-16**.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fputs16(tro_file *file, const tro_char16 *s);

/**
 * Escreve os dados do *buffer interno* no
 * arquivo real e esvazia o *buffer interno**.
 * Não tem efeito quando o **modo de gerenciamento**
 * do *buffer interno* do @ref tro_file é
 * @ref TRO_FBUFMODE_NO_BUFFER.
 *
 * @param file o objeto.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fflush(tro_file *file);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param file o objeto.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fprintf(tro_file *file, const char *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_fprintf.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param file o objeto.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_vfprintf(tro_file *file, const char *format, va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param file o objeto.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_fprintf16(tro_file *file, const tro_char16 *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_fprintf16.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param file o objeto.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_vfprintf16(tro_file *file, const tro_char16 *format,
                             va_list args);

/**
 * Representação da **saída padrão**.
 * O **modo de gerenciamento** do *buffer
 * interno* é @ref TRO_FBUFMODE_LINE_BUFFER.
 * Normalmente é um **console/terminal** a
 * menos que tenha sido *redirecionado* para
 * um arquivo.
 *
 * @warning
 * Este objeto **é gerenciada pela LibTRO**,
 * portanto você **NÃO** deve usar @ref tro_fclose
 * neste arquivo pois **pode causar comportamentos
 * indefinidos e imprevisíveis**.
 */
extern tro_file *const troout;
/**
 * Representação da **entrada padrão**.
 * Normalmente é um **console/terminal** a
 * menos que tenha sido *redirecionado* para
 * um arquivo.
 *
 * @warning
 * Este objeto **é gerenciada pela LibTRO**,
 * portanto você **NÃO** deve usar @ref tro_fclose
 * neste arquivo pois **pode causar comportamentos
 * indefinidos e imprevisíveis**.
 */
extern tro_file *const troin;
/**
 * Representação da **saída de erro padrão**.
 * O **modo de gerenciamento** do *buffer
 * interno* é @ref TRO_FBUFMODE_LINE_BUFFER.
 * Normalmente é um **console/terminal** a
 * menos que tenha sido *redirecionado* para
 * um arquivo.
 *
 * @warning
 * Este objeto **é gerenciada pela LibTRO**,
 * portanto você **NÃO** deve usar @ref tro_fclose
 * neste arquivo pois **pode causar comportamentos
 * indefinidos e imprevisíveis**.
 */
extern tro_file *const troerr;

/**
 * Escreve uma **runa** em @ref troout.
 *
 * @param rune é a **runa** (**code point**).
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_putc(tro_urune rune);

/**
 * Escreve uma *string* terminada
 * em `'\0'` em @ref troout.
 *
 * @param s é **string UTF-8**.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_puts(const char *s);

/**
 * Escreve uma *string* terminada
 * em `'\0'` em @ref troout.
 *
 * @param s é **string UTF-16**.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_puts16(const tro_char16 *s);

/**
 * Escreve uma **runa** em @ref troerr.
 *
 * @param rune é a **runa** (**code point**).
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_eputc(tro_urune rune);

/**
 * Escreve uma *string* terminada
 * em `'\0'` em @ref troerr.
 *
 * @param s é **string UTF-8**.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_eputs(const char *s);

/**
 * Escreve uma *string* terminada
 * em `'\0'` em @ref troerr.
 *
 * @param s é **string UTF-16**.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_eputs16(const tro_char16 *s);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troout.
 *
 * @param format é a **string de formatação** em **UTF-8**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_printf(const char *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_printf.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troout.
 *
 * @param format é a **string de formatação** em **UTF-8**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_vprintf(const char *format, va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troout.
 *
 * @param format é a **string de formatação** em **UTF-16**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_printf16(const tro_char16 *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_printf.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troout.
 *
 * @param format é a **string de formatação** em **UTF-16**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_vprintf16(const tro_char16 *format, va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troerr.
 *
 * @param format é a **string de formatação** em **UTF-8**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_eprintf(const char *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_eprintf.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troerr.
 *
 * @param format é a **string de formatação** em **UTF-8**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_veprintf(const char *format, va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troerr.
 *
 * @param format é a **string de formatação** em **UTF-16**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_eprintf16(const tro_char16 *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_eprintf16.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec e
 * escreve em @ref troerr.
 *
 * @param format é a **string de formatação** em **UTF-16**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_veprintf16(const tro_char16 *format, va_list args);

TRO__C_API_END

#endif // TRO_TROIO_H_
