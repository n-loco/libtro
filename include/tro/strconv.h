/** @file */

#ifndef TRO_STRCONV_H_
#define TRO_STRCONV_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

#include <stddef.h>
#include <stdint.h>

#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif

#include "tro/uchar.h"

TRO__C_API_BEGIN

/**
 * @def TRO_INT_CHAR_MAX
 *
 * Comprimento **máximo** que uma
 * **string** pode chegar em uma
 * conversão de um *inteiro* para
 * uma *strint decimal* desconsiderando
 * o `'\0'` no final.
 *
 * @see tro_int2str
 * @see tro_int2str16
 */
#define TRO_INT_CHAR_MAX 21

/**
 * @def TRO_UINT_CHAR_MAX
 *
 * Comprimento **máximo** que uma
 * **string** pode chegar em uma
 * conversão de um *inteiro sem
 * sinal* para  uma *strint decimal*
 * desconsiderando o `'\0'` no final.
 *
 * @see tro_uint2str
 * @see tro_uint2str16
 */
#define TRO_UINT_CHAR_MAX 20

/**
 * Faz uma conversão de um *inteiro* para uma
 * **string decimal UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_INT_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_int2str(int64_t num, char *out, size_t outcap);

/**
 * Faz uma conversão de um *inteiro sem sinal* para uma
 * **string decimal UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_UINT_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_uint2str(uint64_t num, char *out, size_t outcap);

/**
 * Faz uma conversão de um *inteiro* para uma
 * **string decimal UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_INT_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_int2str16(int64_t num, tro_char16 *out, size_t outcap);

/**
 * Faz uma conversão de um *inteiro sem sinal* para uma
 * **string decimal UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_UINT_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_uint2str16(uint64_t num, tro_char16 *out, size_t outcap);

/**
 * @def TRO_FLOAT_CHAR_MAX_PRECISION
 *
 * Precisão **máxima** suportada por
 * funções de conversão de *número de
 * ponto flutuante* para *string*.
 */
#define TRO_FLOAT_CHAR_MAX_PRECISION 17

/**
 * @def TRO_FLOAT_FIXED_CHAR_MAX
 *
 * Comprimento **máximo** que uma
 * **string** pode chegar em uma
 * conversão de um *número de ponto
 * flutuante* para uma *strint de um
 * número real* desconsiderando
 * o `'\0'` no final.
 *
 * @see tro_float2str
 * @see tro_float2str_adjustable
 * @see tro_float2str_fixed
 * @see tro_float2str16
 * @see tro_float2str16_adjustable
 * @see tro_float2str16_fixed
 */
#define TRO_FLOAT_FIXED_CHAR_MAX (2 + 309 + TRO_FLOAT_CHAR_MAX_PRECISION)

/**
 * @def TRO_FLOAT_E_CHAR_MAX
 *
 * Comprimento **máximo** que uma
 * **string** pode chegar em uma
 * conversão de um *número de ponto
 * flutuante* para uma *strint de um
 * número em notação E* desconsiderando
 * o `'\0'` no final.
 *
 * @see tro_float2str_E
 * @see tro_float2str_e
 * @see tro_float2str_fixed_E
 * @see tro_float2str_fixed_e
 * @see tro_float2str16_E
 * @see tro_float2str16_e
 * @see tro_float2str16_fixed_E
 * @see tro_float2str16_fixed_e
 */
#define TRO_FLOAT_E_CHAR_MAX 25

/**
 * Faz uma conversão de um *número de ponto flutuante*
 * para uma **string de número real UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_FIXED_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str(double num, char *out, size_t outcap);

/**
 * Faz uma conversão com **precisão ajustável**
 * de um *número de ponto flutuante* para
 * uma **string de número real UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_FIXED_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str_adjustable(double num, uint32_t precision,
                                         char *out, size_t outcap);

/**
 * Faz uma conversão com **casas fixas de fração**
 * de um *número de ponto flutuante* para
 * uma **string de número real UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_FIXED_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str_fixed(double num, uint32_t precision, char *out,
                                    size_t outcap);

/**
 * Faz uma conversão de um *número de ponto flutuante*
 * para uma **string de número em notação E UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str_E(double num, char *out, size_t outcap);

/**
 * Faz uma conversão de um *número de ponto flutuante*
 * para uma **string de número em notação E (minúsculo) UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str_e(double num, char *out, size_t outcap);

/**
 * Faz uma conversão com **precisão fixa** de um
 * *número de ponto flutuante* para uma
 * **string de número em notação E UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str_fixed_E(double num, uint32_t precision, char *out,
                                      size_t outcap);

/**
 * Faz uma conversão com **precisão fixa** de um
 * *número de ponto flutuante* para uma
 * **string de número em notação E (minúsculo) UTF-8**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str_fixed_e(double num, uint32_t precision, char *out,
                                      size_t outcap);

/**
 * Faz uma conversão de um *número de ponto flutuante*
 * para uma **string de número real UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_FIXED_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str16(double num, tro_char16 *out, size_t outcap);

/**
 * Faz uma conversão com **precisão ajustável**
 * de um *número de ponto flutuante* para
 * uma **string de número real UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_FIXED_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str16_adjustable(double num, uint32_t precision,
                                           tro_char16 *out, size_t outcap);

/**
 * Faz uma conversão com **casas fixas de fração**
 * de um *número de ponto flutuante* para
 * uma **string de número real UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_FIXED_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str16_fixed(double num, uint32_t precision,
                                      tro_char16 *out, size_t outcap);

/**
 * Faz uma conversão de um *número de ponto flutuante*
 * para uma **string de número em notação E UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str16_E(double num, tro_char16 *out, size_t outcap);

/**
 * Faz uma conversão de um *número de ponto flutuante*
 * para uma **string de número em notação E (minúsculo) UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str16_e(double num, tro_char16 *out, size_t outcap);

/**
 * Faz uma conversão com **precisão fixa** de um
 * *número de ponto flutuante* para uma
 * **string de número em notação E UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str16_fixed_E(double num, uint32_t precision,
                                        tro_char16 *out, size_t outcap);

/**
 * Faz uma conversão com **precisão fixa** de um
 * *número de ponto flutuante* para uma
 * **string de número em notação E (minúsculo) UTF-16**.
 *
 * @param num é o *número* em questão.
 * @param precision é a *precisão* em casas da fração.
 * @param out é onde a *string* será escrita.
 * @param outcap é a capacidade de `out`,
 * idealmente `TRO_FLOAT_E_CHAR_MAX + 1`.
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_float2str16_fixed_e(double num, uint32_t precision,
                                        tro_char16 *out, size_t outcap);

TRO__C_API_END

#endif // TRO_STRCONV_H_
