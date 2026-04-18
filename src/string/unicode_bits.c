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
   há macros definidas para ele em UTF-8 e UTF-16 (REPLACE e REPLACE_U*_*).

   RFC UTF-8:  https://www.rfc-editor.org/rfc/rfc3629
   RFC UTF-16: https://www.rfc-editor.org/rfc/rfc2781
*/
// clang-format on

#include "tro/string.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define REPLACE 0x00FFFD

#define URUNE_IS_U16_RESERVED(rune) (0xD800 <= rune && rune <= 0xDFFF)

#define URUNE_IS_VALID(rune)                                                   \
	((!URUNE_IS_U16_RESERVED(rune)) && rune <= TRO_URUNE_MAX)

#define REPLACE_U8_0 0xEF
#define REPLACE_U8_1 0xBF
#define REPLACE_U8_2 0xBD

#define URUNE_CALC_U8_SIZE(rune)                                               \
	((size_t)((rune > 0xFFFF) + (rune > 0x7FF) + (rune > 0x7F) + 1))

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

static inline size_t seq_len_u8(const tro_u8code *seq)
{
	if (seq[0] == '\0')
		return 0;

	if (seq[1] == '\0')
		return 1;

	if (seq[2] == '\0')
		return 2;

	if (seq[3] == '\0')
		return 3;

	return 4;
}

#define U8_LEN(seq)                                                            \
	((((seq[0] & 0xF8) == 0xF0) * 4) + (((seq[0] & 0xF0) == 0xE0) * 3) +   \
	 (((seq[0] & 0xE0) == 0xC0) * 2) + (((seq[0] & 0x80) == 0x00) * 1))

#define U8_IS_CONT(u8) ((u8 & 0xC0) == 0x80)

static inline size_t u8_seq_valid_until(size_t u8_l, const tro_u8code *seq)
{
	size_t v;
	for (v = 1; v < u8_l; v++) {
		if (!U8_IS_CONT(seq[v]))
			break;
	}
	return v;
}

#define RUNE_U8_0(len, seq)                                                    \
	(((len == 1) * seq[0]) + ((len == 2) * ((seq[0] & 0x1F) << 6)) +       \
	 ((len == 3) * ((seq[0] & 0x0F) << 12)) +                              \
	 ((len == 4) * ((seq[0] & 0x07) << 18)))

#define RUNE_U8_1(len, seq)                                                    \
	(((len == 2) * (seq[1] & 0x3F)) +                                      \
	 ((len == 3) * ((seq[1] & 0x3F) << 6)) +                               \
	 ((len == 4) * ((seq[1] & 0x3F) << 12)))

#define RUNE_U8_2(len, seq)                                                    \
	(((len == 3) * (seq[2] & 0x3F)) + ((len == 4) * ((seq[2] & 0x3F) << 6)))

#define RUNE_U8_3(len, seq) ((len == 4) * (seq[3] & 0x3F))

size_t tro_u8codes_to_urune(const tro_u8code *seq, size_t seq_l, tro_urune *out)
{
	if (seq_l == 0)
		seq_l = seq_len_u8(seq);
	if (seq_l == 0)
		return 0;

	size_t u8_l = U8_LEN(seq);

	if (u8_l == 0)
		goto ILLEGAL_BYTE;

	if (seq_l < u8_l)
		goto SEQ_TOO_SMALL;

	tro_u8code rseq[] = {
	    seq[0],
	    (u8_l > 1 ? seq[1] : 0),
	    (u8_l > 2 ? seq[2] : 0),
	    (u8_l > 3 ? seq[3] : 0),
	};

	size_t vu = u8_seq_valid_until(u8_l, rseq);
	if (vu < u8_l)
		goto INVALID_SEQ;

	tro_urune rune = RUNE_U8_0(u8_l, rseq) | RUNE_U8_1(u8_l, rseq) |
	                 RUNE_U8_2(u8_l, rseq) | RUNE_U8_3(u8_l, rseq);

	if (!URUNE_IS_VALID(rune) || (URUNE_CALC_U8_SIZE(rune) < u8_l))
		goto OVERLONG;

	*out = rune;
	return u8_l;

OVERLONG:
	*out = REPLACE;
	return u8_l;

INVALID_SEQ:
	*out = REPLACE;
	return vu;

SEQ_TOO_SMALL:
	*out = REPLACE;
	return seq_l;

ILLEGAL_BYTE:
	*out = REPLACE;
	return 1;
}

size_t tro_str8_urune_len(const char *str, size_t str_l)
{
	if (str_l == 0)
		str_l = strlen(str);

	size_t rune_count = 0;

	size_t i = 0;
	while (i < str_l) {
		const tro_u8code *seq = (tro_u8code *)(str + i);
		const size_t seq_len  = str_l - i;

		size_t u8_len = U8_LEN(seq);
		if (u8_len == 0 || u8_len == 1) {
			i++;
			rune_count++;
			continue;
		}

		if (seq_len < u8_len) {
			rune_count++;
			break;
		}

		size_t valid_until = u8_seq_valid_until(u8_len, seq);
		if (valid_until < u8_len) {
			i += valid_until;
			rune_count++;
			continue;
		}

		i += u8_len;
		rune_count++;
	}

	return rune_count;
}

#define REPLACE_U16_W1 0xFFFD

#define URUNE_CALC_U16_SIZE(rune) ((rune > 0xFFFF) + 1)

#define U16CODE_W1(len, rune)                                                  \
	(((len == 1) * rune) + ((len == 2) * ((rune >> 10) | 0xD800)))

#define U16CODE_W2(len, rune) ((len == 2) * ((rune & 0x3FF) | 0xDC00))

size_t tro_urune_to_u16codes(tro_urune rune, tro_u16code *out)
{
	bool vr      = URUNE_IS_VALID(rune);
	size_t ru16s = (vr * URUNE_CALC_U16_SIZE(rune)) + (!vr * 1);
	tro_urune Ul = rune - ((ru16s == 2) * 0x10000);

	out[0] = (vr * U16CODE_W1(ru16s, Ul)) + (!vr * REPLACE_U16_W1);
	out[1] = vr * U16CODE_W2(ru16s, Ul);

	return ru16s;
}

static inline size_t seq_len_u16(const tro_u16code *seq)
{
	if (seq[0] == '\0')
		return 0;

	if (seq[1] == '\0')
		return 1;

	return 2;
}

#define U16_IS_HIGH(w1) (0xD800 <= w1 && w1 <= 0xDBFF)
#define U16_IS_LOW(w2) (0xDC00 <= w2 && w2 <= 0xDFFF)

#define U16_LEN(seq) (U16_IS_HIGH(seq[0]) + 1)

size_t tro_u16codes_to_urune(const tro_u16code *seq, size_t seq_len,
                             tro_urune *out)
{
	if (seq_len == 0)
		seq_len = seq_len_u16(seq);
	if (seq_len == 0)
		return 0;

	size_t u16_l = U16_LEN(seq);

	if (seq_len < u16_l)
		goto SEQ_TOO_SMALL_OR_INVALID_SEQ;

	if (u16_l > 1) {
		if (!U16_IS_LOW(seq[1]))
			goto SEQ_TOO_SMALL_OR_INVALID_SEQ;

		*out = (((seq[0] & 0x3FF) << 10) | (seq[1] & 0x3FF)) + 0x10000;
		return 2;
	}

	*out = (tro_urune)seq[0];
	return 1;

SEQ_TOO_SMALL_OR_INVALID_SEQ:
	*out = REPLACE;
	return 1;
}

size_t tro_str16_urune_len(const char16_t *str, size_t str_l)
{
	if (str_l == 0)
		str_l = tro_str16len(str);

	size_t rune_count = 0;

	size_t i = 0;
	while (i < str_l) {
		const tro_u16code *seq = (tro_u16code *)(str + i);
		const size_t seq_len   = str_l - i;

		size_t u16_len = U16_LEN(seq);
		if (u16_len == 1) {
			i++;
			rune_count++;
			continue;
		}

		if (seq_len < u16_len) {
			rune_count++;
			break;
		}

		if (!U16_IS_LOW(seq[1])) {
			i++;
			rune_count++;
			continue;
		}

		i += 2;
		rune_count++;
	}

	return rune_count;
}
