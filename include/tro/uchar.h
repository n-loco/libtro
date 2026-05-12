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
 * @see tro_u18codes_to_u8codes
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
 * @param rune deve ser um **número**.
 *
 * @see tro_urune
 */
#define TRO_URUNE_IS_VALID(rune)                                               \
	((rune < 0xD800 || 0xDFFF < rune) && rune <= TRO_URUNE_MAX)

TRO__API size_t tro_urune_to_u8codes(tro_urune rune, tro_u8code *out);

TRO__API size_t tro_u8codes_to_urune(const tro_u8code *seq, size_t seqlen,
                                     tro_urune *out);

TRO__API size_t tro_urune_to_u16codes(tro_urune rune, tro_u16code *out);

TRO__API size_t tro_u16codes_to_urune(const tro_u16code *seq, size_t seqlen,
                                      tro_urune *out);

TRO__API size_t tro_u8codes_to_u16codes(const tro_u8code *seq, size_t seqlen,
                                        tro_u16code *out, size_t *outlen);

TRO__API size_t tro_u16codes_to_u8codes(const tro_u16code *seq, size_t seqlen,
                                        tro_u8code *out, size_t *outlen);

typedef uint16_t tro_char16;

TRO__API size_t tro_str8_urune_len(const char *str, size_t strl);

TRO__API size_t tro_str16_urune_len(const tro_char16 *str, size_t strl);

TRO__C_API_END

#endif // TRO_UCHAR_H_
