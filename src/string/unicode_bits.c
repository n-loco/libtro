// clang-format off
/*
   Este arquivo contém a manipulação Unicode low level de bits.

   O motivo do porque isto está separado é para dividir entre
   código Unicode "normal" e código Unicode "low-level", pois
   aqui se tomou a liberdade de usar técnicas não tão boas para
   leitura, mas que favorecem otimizações, como branchless.

   Aqui como os formatos UTF-8 e UTF-16 funcionam:

   |============================ UTF-8 =============================|
   Bytes |     Raios (H)     |            Layout UTF-8 (B)
   ------|-------------------|---------------------------------------
       1 | 00000000-0000007F | 0xxxxxxx
       2 | 00000080-000007FF | 110xxxxx  10xxxxxx
       3 | 00000800-0000FFFF | 1110xxxx  10xxxxxx  10xxxxxx
       4 | 00010000-0010FFFF | 11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
   ------------------------------------------------------------------

   |============================= UTF-16 ==============================|
   Surrogates |     Raios (H)     |          Layout UTF-16 (B)
   -----------|-------------------|-------------------------------------
            1 | 00000000-0000FFFF | xxxxxxxx xxxxxxxx
            2 | 00010000-0010FFFF | 110110xx xxxxxxxx  110111xx xxxxxxxx
   ---------------------------------------------------------------------

   Importante notar que o raio 0xD800-0xDFFF é reservado para a
   codificação UTF-16 de 2 surrogates, sendo 0xD800-0xDBFF para o
   primeiro surrogate e 0xDC00-0xDFFF para o segundo surrogate, portanto,
   não há nenhum code point dentro deste raio, então codificações UTF-8
   ou code points Unicode puros neste raio são considerados inválidos.

   Também importante notar que para o número caber dentro do par de
   surrogates em UTF-16, ele deve ser subtraído por 0x10000, assim
   cabendo dentro dos 20 bits oferecidos pelo par.

   Em casos de erro de encodificação/decodificação, se usa o
   '�' (REPLACEMENT CHARACTER), seu código Unicode é U+FFFD e aqui
   há macros definidas para ele em UTF-8 e UTF-16 (REPLACE_U*_*).

   RFC UTF-8:  https://www.rfc-editor.org/rfc/rfc3629
   RFC UTF-16: https://www.rfc-editor.org/rfc/rfc2781
*/
// clang-format on

#include "tro/string.h"

#include <stddef.h>
#include <stdbool.h>

#define URUNE_IS_U16_RESERVED(rune) (0xD800 <= rune && rune <= 0xDFFF)

#define URUNE_IS_VALID(rune)                                                   \
	((!URUNE_IS_U16_RESERVED(rune)) && rune <= TRO_URUNE_MAX)

#define REPLACE_U8_0 0xEF
#define REPLACE_U8_1 0xBF
#define REPLACE_U8_2 0xBD

#define URUNE_CALC_U8_SIZE(rune)                                               \
	((rune > 0xFFFF) + (rune > 0x7FF) + (rune > 0x7F) + 1)

#define U8CODE_0(len, rune)                                                    \
	(((len == 1) * rune) + ((len == 2) * ((rune >> 6) | 0xC0)) +           \
	 ((len == 3) * ((rune >> 12) | 0xE0)) +                                \
	 ((len == 4) * ((rune >> 18) | 0xF0)))

#define U8CODE_1(len, rune)                                                    \
	(((len == 2) * ((rune & 0x3F) | 0x80)) +                               \
	 ((len == 3) * (((rune >> 6) & 0x3F) | 0x80)) +                        \
	 ((len == 4) * (((rune >> 12) & 0x3F) | 0x80)))

#define U8CODE_2(len, rune)                                                    \
	(((len == 3) * ((rune & 0x3F) | 0x80)) +                               \
	 ((len == 4) * (((rune >> 6) & 0x3F) | 0x80)))

#define U8CODE_3(len, rune) ((len == 4) * ((rune & 0x3F) | 0x80))

size_t tro_urune_to_u8codes(tro_urune rune, tro_u8code *out)
{
	bool vr     = URUNE_IS_VALID(rune);
	size_t ru8s = (vr * URUNE_CALC_U8_SIZE(rune)) + (!vr * 3);

	out[0] = (vr * U8CODE_0(ru8s, rune)) + (!vr * REPLACE_U8_0);
	out[1] = (vr * U8CODE_1(ru8s, rune)) + (!vr * REPLACE_U8_1);
	out[2] = (vr * U8CODE_2(ru8s, rune)) + (!vr * REPLACE_U8_2);
	out[3] = vr * U8CODE_3(ru8s, rune);

	return ru8s;
}

#define REPLACE_U16_W1 0xFFFD

#define URUNE_CALC_U16_SIZE(rune) ((rune > 0xFFFF) + 1)

#define U16CODE_W1(len, rune)                                                  \
	(((len == 1) * rune) + ((len == 2) * ((rune >> 10) | 0xD800)))

#define U16CODE_W2(len, rune) ((len == 2) * ((rune & 0x3FF) | 0xDC00))

size_t tro_urune_to_u16codes(tro_urune rune, tro_u16code *out)
{
	bool vr         = URUNE_IS_VALID(rune);
	size_t ru16s    = (vr * URUNE_CALC_U16_SIZE(rune)) + (!vr * 1);
	tro_urune Ul = rune - ((ru16s == 2) * 0x10000);

	out[0] = (vr * U16CODE_W1(ru16s, Ul)) + (!vr * REPLACE_U16_W1);
	out[1] = vr * U16CODE_W2(ru16s, Ul);

	return ru16s;
}
