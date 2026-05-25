/** @file */

#ifndef TRO_DYBUFFER_H_
#define TRO_DYBUFFER_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"
#include "tro/private/doxy.h"

#include <stddef.h>
#include <stdint.h>
#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif

#include "tro/uchar.h"

TRO__C_API_BEGIN

/**
 * Representa a **preferência** de
 * *encoding* de um objeto @ref tro_dybuffer_obj.
 *
 * @see tro_dybuffer_i
 */
typedef enum tro__ndoxy(tro_dybuf_pref) {
	/**
	 * Significa que @ref tro_dybuffer_obj
	 * **NÃO** tem nenhuma preferência de
	 * encoding.
	 */
	TRO_DYBUF_PREF_ANY,
	/**
	 * Significa que @ref tro_dybuffer_obj
	 * tem **UTF-8** como *encoding preferencial*.
	 */
	TRO_DYBUF_PREF_U8,
	/**
	 * Significa que @ref tro_dybuffer_obj
	 * tem **UTF-16** como *encoding preferencial*.
	 */
	TRO_DYBUF_PREF_U16,
} tro_dybuf_pref;

/**
 * Estrutura da **tabela virtual** que
 * representa a **interface** *tro_dybuffer*,
 * que significa *"buffer dinâmico"*.
 *
 * @see tro_dybuffer_obj
 */
typedef struct tro__ndoxy(tro_dybuffer_i) {
	/**
	 * @param buf o objeto.
	 * @param data é uma **string UTF-8**.
	 * @param datal é o comprimento de `data`,
	 * caso seja `0`, se assume `data` terminado
	 * em `'\0'`.
	 *
	 * @returns `true` se sucedido, `false` se falhou.
	 */
	bool (*const writes)(void *buf, const char *data, size_t datal);
	/**
	 * @param buf o objeto.
	 * @param data é uma **string UTF-16**.
	 * @param datal é o comprimento de `data`,
	 * caso seja `0`, se assume `data` terminado
	 * em `'\0'`.
	 *
	 * @returns `true` se sucedido, `false` se falhou.
	 */
	bool (*const writes16)(void *buf, const tro_char16 *data, size_t datal);
	/**
	 * @param buf o objeto.
	 * @param data é uma **sequência de bytes arbitrária**.
	 * @param datal é o comprimento de `data`.
	 *
	 * @returns `true` se sucedido, `false` se falhou.
	 */
	bool (*const writeb)(void *buf, const uint8_t *data, size_t datal);
	/**
	 * @param buf o objeto.
	 * @param rune é uma **runa** (**code point**).
	 * @param count diz quantas vezes se deve escrever `rune`.
	 *
	 * @returns `true` se sucedido, `false` se falhou.
	 */
	bool (*const writec)(void *buf, tro_urune rune, size_t count);
	/**
	 * @param buf o objeto.
	 *
	 * @returns a *preferência de encoding*.
	 */
	tro_dybuf_pref (*const preference)(const void *buf);
} tro_dybuffer_i;

/**
 * Um **ponteiro gordo** para uma
 * instância real de um objeto
 * `tro_dybuffer`.
 * **NUNCA** deve ser **passado
 * como um ponteiro**.
 */
typedef struct tro__ndoxy(tro_dybuffer_obj) {
	/**
	 * Um **ponteiro opaco** para
	 * o objeto real.
	 */
	void *const obj;
	/**
	 * Um **ponteiro** para a
	 * **tabela virtual** de `obj`
	 * com as implementações de
	 * @ref tro_dybuffer_i.
	 */
	const tro_dybuffer_i *const vtable;
} tro_dybuffer_obj;

/**
 * @def tro_dispatch_dybuffer(o, varname)
 *
 * @param o é o @ref tro_dybuffer_obj a se
 * **fazer o dispacho** dos *métodos*.
 * @param varname é o nome da **constante**
 * que representa o *objeto real* e também o
 * **prefixo** dos **métodos dispachados**:
 * ```
 * <varname>            = ... ;
 * <varname>_writes     = ... ;
 * <varname>_writes16   = ... ;
 * <varname>_writeb     = ... ;
 * <varname>_writec     = ... ;
 * <varname>_preference = ... ;
 * ```
 */
#define tro_dispatch_dybuffer(o, varname)                                      \
	void *const varname = o.obj;                                           \
                                                                               \
	bool (*const varname##_writes)(void *buf, const char *data,            \
	                               size_t datal) = o.vtable->writes;       \
                                                                               \
	bool (*const varname##_writes16)(void *buf, const tro_char16 *data,    \
	                                 size_t datal) = o.vtable->writes16;   \
                                                                               \
	bool (*const varname##_writeb)(void *buf, const uint8_t *data,         \
	                               size_t datal) = o.vtable->writeb;       \
                                                                               \
	bool (*const varname##_writec)(void *buf, tro_urune rune,              \
	                               size_t count) = o.vtable->writec;       \
                                                                               \
	tro_dybuf_pref (*const varname##_preference)(const void *buf) =        \
	    o.vtable->preference;                                              \
                                                                               \
	/* Supressão de warnings. */ {                                         \
		(void)varname;                                                 \
		(void)varname##_writes;                                        \
		(void)varname##_writes16;                                      \
		(void)varname##_writeb;                                        \
		(void)varname##_writec;                                        \
		(void)varname##_preference;                                    \
	}

TRO__C_API_END

#endif // TRO_DYBUFFER_H_
