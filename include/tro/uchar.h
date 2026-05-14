/** @file */

#ifndef TRO_UCHAR_H_
#define TRO_UCHAR_H_

#include "tro/private/api.h"

#include <stdint.h>
#include <stddef.h>

TRO__C_API_BEGIN

/**
 * Representa **um** único **byte** de
 * uma sequência **UTF-8**.
 */
typedef uint8_t tro_u8code;

/**
 * @def TRO_MULTI_U8CODE_MAX
 *
 * Define o **máximo** que um @ref tro_urune
 * pode chegar como uma sequência @ref tro_u8code.
 *
 * @see tro_urune_to_u8codes
 * @see tro_u16codes_to_u8codes
 */
#define TRO_MULTI_U8CODE_MAX 4

/**
 * Representa **um** único **surrogate**
 * (*número de 16 bits*) de uma
 * sequência **UTF-16**.
 */
typedef uint16_t tro_u16code;

/**
 * @def TRO_MULTI_U16CODE_MAX
 *
 * Define o **máximo** que @ref tro_urune
 * pode chegar como uma sequência @ref tro_u16code.
 *
 * @see tro_urune_to_u16codes
 * @see tro_u8codes_to_u16codes
 */
#define TRO_MULTI_U16CODE_MAX 2

/**
 * Representa um **code point Unicode**.
 */
typedef uint32_t tro_urune;

/**
 * @def TRO_URUNE_MAX
 *
 * Define o valor máximo de @ref tro_urune,
 * ou seja, o **último code point**.
 */
#define TRO_URUNE_MAX 0x10FFFF

/**
 * @def TRO_URUNE_IS_VALID(rune)
 *
 * Retorna se `rune` é um **code point**
 * realmente **válido**.
 *
 * @param rune deve ser um **número**,
 * de preferência um @ref tro_urune.
 */
#define TRO_URUNE_IS_VALID(rune)                                               \
	((rune < 0xD800 || 0xDFFF < rune) && rune <= TRO_URUNE_MAX)

/**
 * Converte um @ref tro_urune para uma
 * sequência @ref tro_u8code.
 * Em caso de **erro**, a sequência
 * codificada será do `u'�'` (*REPLACEMENT CHARACTER*).
 *
 * @param rune é a runa com o **code point**.
 * @param out é um array de tamanho @ref TRO_MULTI_U8CODE_MAX.
 *
 * @returns o tamanho da sequência final.
 *
 * @see tro_u8codes_to_urune
 * @see tro_u16codes_to_u8codes
 */
TRO__API size_t tro_urune_to_u8codes(tro_urune rune, tro_u8code *out);

/**
 * Converte uma sequência @ref tro_u8code
 * para um @ref tro_urune.
 * Em caso de **erro**, a **runa** será
 * `u'�'` (*REPLACEMENT CHARACTER*).
 *
 * @param seq é a sequência **UTF-8** para decodificar.
 * @param seqlen é o **comprimento máximo** da sequência.
 * Se for `0`, se assume `seq` terminado em `'\0'`.
 * @param out é um **ponteiro** para armazenar a **runa**
 * decodificada.
 *
 * @returns quantidade de **bytes** lidos.
 *
 * @see tro_urune_to_u8codes
 * @see tro_urune_to_u16codes
 */
TRO__API size_t tro_u8codes_to_urune(const tro_u8code *seq, size_t seqlen,
                                     tro_urune *out);

/**
 * Converte um @ref tro_urune para uma
 * sequência @ref tro_u16code.
 * Em caso de **erro**, a sequência
 * codificada será do `u'�'` (*REPLACEMENT CHARACTER*).
 *
 * @param rune é a runa com o **code point**.
 * @param out é um array de tamanho @ref TRO_MULTI_U16CODE_MAX.
 *
 * @returns o tamanho da sequência final.
 *
 * @see tro_u8codes_to_u16codes
 * @see tro_u16codes_to_urune
 */
TRO__API size_t tro_urune_to_u16codes(tro_urune rune, tro_u16code *out);

/**
 * Converte uma sequência @ref tro_u16code
 * para um @ref tro_urune.
 * Em caso de **erro**, a **runa** será
 * `u'�'` (*REPLACEMENT CHARACTER*).
 *
 * @param seq é a sequência **UTF-16** para decodificar.
 * @param seqlen é o **comprimento máximo** da sequência.
 * Se for `0`, se assume `seq` terminado em `'\0'`.
 * @param out é um **ponteiro** para armazenar a **runa**
 * decodificada.
 *
 * @returns quantidade de **surrogates** lidos.
 *
 * @see tro_urune_to_u8codes
 * @see tro_urune_to_u16codes
 */
TRO__API size_t tro_u16codes_to_urune(const tro_u16code *seq, size_t seqlen,
                                      tro_urune *out);
/**
 * Converte uma sequência @ref tro_u8code
 * **direto** para uma sequência @ref tro_u16code.
 * Em caso de **erro**, a sequência
 * codificada será do `u'�'` (*REPLACEMENT CHARACTER*).
 *
 * @param seq é a sequência **UTF-8** para decodificar.
 * @param seqlen é o **comprimento máximo** da sequência.
 * Se for `0`, se assume `seq` terminado em `'\0'`.
 * @param out é um array de tamanho @ref TRO_MULTI_U16CODE_MAX.
 * @param outlen é um **ponteiro** para armazenar o tamanho
 * da sequência final.
 *
 * @returns quantidade de **bytes** lidos.
 *
 * @see tro_urune_to_u16codes
 * @see tro_u16codes_to_u8codes
 */
TRO__API size_t tro_u8codes_to_u16codes(const tro_u8code *seq, size_t seqlen,
                                        tro_u16code *out, size_t *outlen);

/**
 * Converte uma sequência @ref tro_u16code
 * **direto** para uma sequência @ref tro_u8code.
 * Em caso de **erro**, a sequência
 * codificada será do `u'�'` (*REPLACEMENT CHARACTER*).
 *
 * @param seq é a sequência **UTF-16** para decodificar.
 * @param seqlen é o **comprimento máximo** da sequência.
 * Se for `0`, se assume `seq` terminado em `'\0'`.
 * @param out é um array de tamanho @ref TRO_MULTI_U8CODE_MAX.
 * @param outlen é um **ponteiro** para armazenar o tamanho
 * da sequência final.
 *
 * @returns quantidade de **surrogates** lidos.
 *
 * @see tro_urune_to_u8codes
 * @see tro_u8codes_to_u16codes
 */
TRO__API size_t tro_u16codes_to_u8codes(const tro_u16code *seq, size_t seqlen,
                                        tro_u8code *out, size_t *outlen);

/**
 * Representa um **caractere UTF-16**
 * e **strings UTF-16** via `tro_char16 *`,
 * em contra partida ao `char`, que
 * **nesta biblioteca** se assume
 * **UTF-8**.
 *
 * @note
 * A biblioteca **não** utiliza `%uchar.h`
 * pela sua ausência em algumas plataformas,
 * mas caso disponível, `tro_char16` é
 * completamente compatível com `char16_t`.
 */
typedef uint16_t tro_char16;

/**
 * @param str é uma **string UTF-8**.
 * @param strl é o comprimento de `str`,
 * caso seja `0`, se assume `str` terminado em `'\0'`.
 *
 * @returns a quantidade de **code points** em `str`.
 */
TRO__API size_t tro_str8_urune_len(const char *str, size_t strl);

/**
 * @param str é uma **string UTF-16**.
 * @param strl é o comprimento de `str`,
 * caso seja `0`, se assume `str` terminado em `'\0'`.
 *
 * @returns a quantidade de **code points** em `str`.
 *
 * @see tro_char16
 */
TRO__API size_t tro_str16_urune_len(const tro_char16 *str, size_t strl);

TRO__C_API_END

#endif // TRO_UCHAR_H_
