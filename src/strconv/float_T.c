#include "tro/strconv.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ryu/ryu.h>

#include "utils/math.h"

#ifdef USE_CHAR16_T
# include <uchar.h>
# define CHAR_T char16_t
# define tro__float2str_general_T tro__float2str16_general
# define tro__float2str_eE_T tro__float2str16_eE
# define tro__float2str_fixed_eE_T tro__float2str16_fixed_eE
#else
# define CHAR_T char
# define tro__float2str_general_T tro__float2str_general
# define tro__float2str_eE_T tro__float2str_eE
# define tro__float2str_fixed_eE_T tro__float2str_fixed_eE
#endif

// clang-format off
/*
   Todo o trabalho pesado de conversão entre números
   reais e string é delegado à biblioteca Ryu, mas
   ainda há a necessidade de explicar as constantes
   de `strconv.h`.

   @============= Layout do `double` (IEEE 754) =============@
   | Posilão & largura |     Descrição     | ID para análise |
   |-------------------|-------------------|-----------------|
   |         maior bit |       Sinal       | S               |
   |  próximos 11 bits |      Expoente     | E               |
   | os outros 52 bits | Mantissa (fração) | M               |
   |---------------------------------------------------------|

   O número é guardado em um formato de "notação
   científica binária", então N = M ⋅ 2ᴱ.

   O expoente possuí um deslocamento (bias) de 1023,
   isso significa que E ∈ [-1022, +1023], sendo:
    - 1023 =    0;
    - 1024 =   +1;
    - 1022 =   -1;
    - 2046 = +1023;
    - 0    = -1022,
   O valor 1024 (2047) é reservado para representar
   os valores ±`Intinity` e `NaN`.

   Teoricamente, M ∈ [1, 2) e Mₘₐₓ = 2 - 2⁻⁵² ≈ 2, porém na
   prática, interpretamos que Mₘₐₓ = 2 mesmo.

   >>> `TRO_FLOAT_CHAR_MAX_PRECISION`
   | Meramente um número arbitrário com uma boa precisão.

   >>> `TRO_FLOAT_FIXED_CHAR_MAX`
   | O número 2 é simplesmente sinal + ponto.
   |
   | O número 309 (Diₘₐₓ) representa o número máximo de
   | digitos que a parte inteira do número pode chegar,
   | para isso é estabelecido o maior número finito
   | oferecido por `double` (Nₘₐₓ) então:
   |
   |           Nₘₐₓ = Mₘₐₓ ⋅ 2ᴱᵐᵃˣ
   |           Nₘₐₓ = 2 ⋅ 2¹⁰²³
   |           Nₘₐₓ = 2¹⁰²⁴
   |           Nₘₐₓ = 1.797693135E308
   |
   |           Diₘₐₓ = log Nₘₐₓ
   |           Diₘₐₓ = log 1.797693135E308
   |           Diₘₐₓ = 308.25471556
   |           Diₘₐₓ ≈ 309
   |
   | Já a adição com a constante `TRO_FLOAT_CHAR_MAX_PRECISION`
   | é para os digitos da parte fracionária.

   >>> `TRO_FLOAT_E_CHAR_MAX`
   | 25 aparentemente é um número padrão quando se usa
   | este tipo de notação.
*/
// clang-format on

size_t tro__float2str_general_T(double num, uint32_t precision, CHAR_T *out,
                                size_t outcap, bool shortest)
{
	if (out != NULL && outcap < 2) {
		if (outcap > 0)
			*out = '\0';
		return 0;
	}

	precision = MIN(precision, TRO_FLOAT_CHAR_MAX_PRECISION);

	char buffer[TRO_FLOAT_FIXED_CHAR_MAX + 1];
	const size_t strl_raw =
	    (size_t)d2fixed_buffered_n(num, precision, buffer);

	size_t strl = strl_raw;

	if (shortest) {
		/*
		   Removemos zeros extras à
		   direita da parte fracionária.
		   (gambiarra)
		*/
		for (size_t i = 0; i < precision; i++) {
			if (buffer[strl - 1] != '0')
				break;

			/*
			   Remove o '.' se precisar.
			   (gambiarra também)
			*/
			if (i == 5)
				strl -= 2;
			else
				strl--;
		}
	}

	if (out == NULL)
		return strl;

	const size_t outcap_bn = outcap - 1;

	size_t wout = 0;

	for (size_t i = 0; i < strl; i++) {
		out[wout++] = (CHAR_T)buffer[i];
		if (wout == outcap_bn)
			break;
	}
	out[wout] = '\0';

	return wout;
}

size_t tro__float2str_eE_T(double num, CHAR_T *out, size_t outcap,
                           bool capital_e)
{
	if (out != NULL && outcap < 2) {
		if (outcap > 0)
			*out = '\0';
		return 0;
	}

	char buffer[TRO_FLOAT_E_CHAR_MAX + 1];
	const size_t strl = (size_t)d2s_buffered_n(num, buffer);

	if (out == NULL)
		return strl;

	if (!capital_e) {
		// Gambiarra.
		for (size_t i = 0; i < strl; i++) {
			if (buffer[i] == 'E') {
				buffer[i] = 'e';
				break;
			}
		}
	}

	const size_t outcap_bn = outcap - 1;

	size_t wout = 0;

	for (size_t i = 0; i < strl; i++) {
		out[wout++] = (CHAR_T)buffer[i];
		if (wout == outcap_bn)
			break;
	}
	out[wout] = '\0';

	return wout;
}

size_t tro__float2str_fixed_eE_T(double num, uint32_t precision, CHAR_T *out,
                                 size_t outcap, bool capital_e)
{
	if (out != NULL && outcap < 2) {
		if (outcap > 0)
			*out = '\0';
		return 0;
	}

	precision = MIN(precision, TRO_FLOAT_CHAR_MAX_PRECISION);

	char buffer[TRO_FLOAT_FIXED_CHAR_MAX + 1];
	const size_t strl = (size_t)d2exp_buffered_n(num, precision, buffer);

	if (out == NULL)
		return strl;

	if (capital_e) {
		// Gambiarra.
		for (size_t i = 0; i < strl; i++) {
			if (buffer[i] == 'e') {
				buffer[i] = 'E';
				break;
			}
		}
	}

	const size_t outcap_bn = outcap - 1;

	size_t wout = 0;

	for (size_t i = 0; i < strl; i++) {
		out[wout++] = (CHAR_T)buffer[i];
		if (wout == outcap_bn)
			break;
	}
	out[wout] = '\0';

	return wout;
}
