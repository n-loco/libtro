// clang-format off
/*
   Este arquivo contém a manipulação Unicode low level de bits.

   O porquê disto estar separado do `unicode.c`, é para dividir entre
   código Unicode "normal" e código Unicode "low-level", pois aqui
   ocorre uma manipulação pesada em nível de bits, além de ter sido
   tomada a liberdade de escrever código menos legível em favor de
   melhorar o campo para otimizações de compilador e de CPU.

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

   Importante notar que o raio 0xD800-0xDFFF é reservado para a codificação
   UTF-16 de 2 surrogates, sendo 0xD800-0xDBFF para o primeiro surrogate
   e 0xDC00-0xDFFF para o segundo surrogate, portanto, não há nenhum code
   point dentro deste raio, então codificações UTF-8 ou code points Unicode
   puros neste raio são considerados inválidos.

   Também importante notar que para o número caber dentro do par de
   surrogates em UTF-16, ele deve ser subtraído por 0x10000, assim
   cabendo dentro dos 20 bits oferecidos pelo par.

   Em casos de erro de encodificação/decodificação, se usa o
   '�' (REPLACEMENT CHARACTER) como substituto.

   RFC UTF-8:  https://www.rfc-editor.org/rfc/rfc3629
   RFC UTF-16: https://www.rfc-editor.org/rfc/rfc2781
*/
// clang-format on

#include "tro/string.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "utils/math.h"

// Code point do '�'.
#define REPLACE_CHAR 0x00FFFD

// Primeiro byte em UTF-8 do '�'.
#define REPLACE_CHAR_U8_0 0xEF
// Segundo byte em UTF-8 do '�'.
#define REPLACE_CHAR_U8_1 0xBF
// Último byte em UTF-8 do '�'.
#define REPLACE_CHAR_U8_2 0xBD

/*
   Primeiro surrogate do '�'
   (igual ao `REPLACE_CHAR`, apenas
   para separar semântica).
*/
#define REPLACE_CHAR_U16_W1 0xFFFD

/*
   Calcula o comprimento de `rune`
   codificado em UTF-8.

   A referência dos valores usados
   são dos raios da tabela do
   comentário do topo.
*/
#define RUNE_LEN_AS_U8(rune)                                                   \
	((size_t)((rune > 0xFFFF) + (rune > 0x7FF) + (rune > 0x7F) + 1))

/*
   Calcula o comprimento de `rune`
   codificado em UTF-16.

   A referência do valor usado
   vem dos raios da tabela do
   comentário do topo.
*/
#define RUNE_LEN_AS_U16(rune) ((size_t)((rune > 0xFFFF) + 1))

/*
   Valor de deslocamento para code
   points até `0x10FFFF` caberem
   dentro dos 20 bits oferecidos pelo
   par de surrogates.
*/
#define U16_OFFSET 0x10000

/*
   Monta o primeiro byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   pré-processado de `rune` em UTF-8.
*/
static inline tro_u8code u8asm_0(size_t len, tro_urune rune);

/*
   Monta o segundo byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   pré-processado de `rune` em UTF-8.
*/
static inline tro_u8code u8asm_1(size_t len, tro_urune rune);

/*
   Monta o terceiro byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   pré-processado de `rune` em UTF-8.
*/
static inline tro_u8code u8asm_2(size_t len, tro_urune rune);

/*
   Monta o quarto byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   pré-processado de `rune` em UTF-8.
*/
static inline tro_u8code u8asm_3(size_t len, tro_urune rune);

size_t tro_urune_to_u8codes(tro_urune rune, tro_u8code *out)
{
	if (!TRO_URUNE_IS_VALID(rune)) {
		out[0] = REPLACE_CHAR_U8_0;
		out[1] = REPLACE_CHAR_U8_1;
		out[2] = REPLACE_CHAR_U8_2;
		out[3] = 0;
		return 3;
	}

	const size_t ulen = RUNE_LEN_AS_U8(rune);

	out[0] = u8asm_0(ulen, rune);
	out[1] = u8asm_1(ulen, rune);
	out[2] = u8asm_2(ulen, rune);
	out[3] = u8asm_3(ulen, rune);

	return ulen;
}

/*
   Retorna o comprimento de uma
   sequência de bytes UTF-8 terminada
   em '\0'. Nunca passa de `4`.
*/
static inline size_t seq8len(const tro_u8code *seq);

/*
   Retorna quantos bytes um code point
   em UTF-8 precisa baseado em `byte`,
   que deve ser o primeiro byte de uma
   sequência UTF-8. Nunca passa de `4`,
   retornar `0` indica erro de byte ilegal.
*/
static inline size_t u8len(const tro_u8code byte);

/*
   Valida os bytes de uma sequência UTF-8
   `seq` de tamanho `len`, assumindo que
   o primeiro byte já é válido e que
   `len` é o comprimento do code point em
   UTF-8.

   Retorna a posição do último byte válido
   da sequência, se for igual a `len`, a
   sequência é válida, caso seja menor que
   `len`, ela é inválida.
*/
static inline size_t u8_validate(const tro_u8code *seq, size_t len);

/*
   Desmonta o primeiro byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   da sequência UTF-8 de onde `byte` veio.
*/
static inline tro_urune u8dasm_0(tro_u8code byte, size_t len);

/*
   Desmonta o segundo byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   da sequência UTF-8 de onde `byte` veio.
*/
static inline tro_urune u8dasm_1(tro_u8code byte, size_t len);

/*
   Desmonta o terceiro byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   da sequência UTF-8 de onde `byte` veio.
*/
static inline tro_urune u8dasm_2(tro_u8code byte, size_t len);

/*
   Desmonta o quarto byte da encodificação
   UTF-8 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   da sequência UTF-8 de onde `byte` veio.
*/
static inline tro_urune u8dasm_3(tro_u8code byte, size_t len);

size_t tro_u8codes_to_urune(const tro_u8code *seq, size_t seqlen,
                            tro_urune *out)
{
	if (seqlen == 0)
		seqlen = seq8len(seq);
	if (seqlen == 0)
		return 0;

	const size_t ulen = u8len(seq[0]);

	if (ulen == 0) { // Byte ilegal.
		*out = REPLACE_CHAR;
		return 1;
	}

	if (seqlen < ulen) {
		*out = REPLACE_CHAR;
		return seqlen;
	}

	const size_t validl = u8_validate(seq, ulen);
	if (validl < ulen) {
		*out = REPLACE_CHAR;
		return validl;
	}

	tro_u8code lseq[] = {
	    seq[0],
	    (ulen > 1 ? seq[1] : 0),
	    (ulen > 2 ? seq[2] : 0),
	    (ulen > 3 ? seq[3] : 0),
	};

	tro_urune rune = u8dasm_0(lseq[0], ulen) | u8dasm_1(lseq[1], ulen) |
	                 u8dasm_2(lseq[2], ulen) | u8dasm_3(lseq[3], ulen);

	bool invalid  = !TRO_URUNE_IS_VALID(rune);
	bool overlong = RUNE_LEN_AS_U8(rune) < ulen;
	if (invalid || overlong) {
		*out = REPLACE_CHAR;
		return ulen;
	}

	*out = rune;
	return ulen;
}

size_t tro_str8_urune_len(const char *str, size_t strl)
{
	if (strl == 0)
		strl = strlen(str);

	size_t rune_count = 0;

	size_t i = 0;
	while (i < strl) {
		const tro_u8code *seq = (tro_u8code *)(str + i);
		const size_t seqlen   = strl - i;

		const size_t ulen = u8len(seq[0]);
		if (ulen == 0 || ulen == 1) {
			i++;
			rune_count++;
			continue;
		}

		if (seqlen < ulen) {
			rune_count++;
			break;
		}

		const size_t validl = u8_validate(seq, ulen);
		if (validl < ulen) {
			i += validl;
			rune_count++;
			continue;
		}

		i += ulen;
		rune_count++;
	}

	return rune_count;
}

/*
   Monta o surogate high da encodificação
   UTF-16 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   pré-processado de `rune` em UTF-16,
   e caso `len == 2`, `rune` é assumido
   com o deslocamento já aplicado.
*/
static inline tro_u16code u16asm_w1(size_t len, tro_urune rune);

/*
   Monta o surogate low da encodificação
   UTF-16 (seguindo os layouts da tabela
   no comentário do topo).

   O parâmetro `len` é o comprimento
   pré-processado de `rune` em UTF-16,
   e caso `len == 2`, `rune` é assumido
   com o deslocamento já aplicado.
*/
static inline tro_u16code u16asm_w2(size_t len, tro_urune rune);

size_t tro_urune_to_u16codes(tro_urune rune, tro_u16code *out)
{
	if (!TRO_URUNE_IS_VALID(rune)) {
		out[0] = REPLACE_CHAR_U16_W1;
		out[1] = 0;
		return 1;
	}

	const size_t ulen = RUNE_LEN_AS_U16(rune);

	/*
	   Se `ulen == 1`, o deslocamento não
	   é aplicado.
	*/
	rune -= (ulen == 2) * U16_OFFSET;

	out[0] = u16asm_w1(ulen, rune);
	out[1] = u16asm_w2(ulen, rune);

	return ulen;
}

/*
   Retorna o comprimento de uma
   sequência de surrogates UTF-16
   terminada em '\0'.

   Únicos valores possíveis são
   `1` e `2`.
*/
static inline size_t seq16len(const tro_u16code *seq)
{
	if (seq[0] == '\0')
		return 0;
	else if (seq[1] == '\0')
		return 1;
	else
		return 2;
}

#define IS_U16HIGH(w1) (0xD800 <= (w1) && (w1) <= 0xDBFF)
#define IS_U16LOW(w2) (0xDC00 <= (w2) && (w2) <= 0xDFFF)

/*
   `surr` é o primeiro surrogate,
*/
#define U16LEN(surr) (IS_U16HIGH(surr) + 1)

#define U16DASM_W1(w1) (((w1) & 0x3FF) << 10)
#define U16DASM_W2(w2) ((w2) & 0x3FF)

size_t tro_u16codes_to_urune(const tro_u16code *seq, size_t seqlen,
                             tro_urune *out)
{
	if (seqlen == 0)
		seqlen = seq16len(seq);
	if (seqlen == 0)
		return 0;

	const size_t ulen = U16LEN(seq[0]);

	if (seqlen < ulen)
		goto ERR;

	if (ulen > 1) {
		if (!IS_U16LOW(seq[1]))
			goto ERR;

		*out = (U16DASM_W1(seq[0]) | U16DASM_W2(seq[1])) + U16_OFFSET;
		return 2;
	}

	*out = seq[0];
	return 1;

ERR:
	*out = REPLACE_CHAR;
	return 1;
}

size_t tro_str16_urune_len(const char16_t *str, size_t strl)
{
	if (strl == 0)
		strl = tro_str16len(str);

	size_t rune_count = 0;

	size_t i = 0;
	while (i < strl) {
		const tro_u16code *seq = (tro_u16code *)(str + i);
		const size_t seqlen   = strl - i;

		const size_t ulen = U16LEN(seq[0]);
		if (ulen == 1) {
			i++;
			rune_count++;
			continue;
		}

		if (seqlen < ulen) {
			rune_count++;
			break;
		}

		if (!IS_U16LOW(seq[1])) {
			i++;
			rune_count++;
			continue;
		}

		i += 2;
		rune_count++;
	}

	return rune_count;
}

static inline tro_u8code u8asm_0(size_t len, tro_urune rune)
{
	return ((len == 1) * rune) // Apenas retorna a própria runa.
	       + ((len == 2) * ((rune >> 6) | 0xC0))  // Para o segundo layout.
	       + ((len == 3) * ((rune >> 12) | 0xE0)) // Para o terceiro layout.
	       + ((len == 4) * ((rune >> 18) | 0xF0)); // Para o último layout.
}
/*
   Macro para montar byte de
   continuação UTF-8.
*/
#define U8CONT(rune) (((rune) & 0x3F) | 0x80)
static inline tro_u8code u8asm_1(size_t len, tro_urune rune)
{
	// Se `len == 1`, só retorna `0x00`
	return ((len == 2) * U8CONT(rune))          // Para o segundo layout.
	       + ((len == 3) * U8CONT(rune >> 6))   // Para o terceiro layout.
	       + ((len == 4) * U8CONT(rune >> 12)); // Para o último layout.
}
static inline tro_u8code u8asm_2(size_t len, tro_urune rune)
{
	// Se `len == 1` ou `len == 2`, só retorna `0x00`
	return ((len == 3) * U8CONT(rune))         // Para o terceiro layout.
	       + ((len == 4) * U8CONT(rune >> 6)); // Para o último layout.
}
static inline tro_u8code u8asm_3(size_t len, tro_urune rune)
{
	/*
	   Só retorna algo se precisar montar o byte
	   do último layout, retorna `0x00` caso contrário.
	*/
	return (len == 4) * U8CONT(rune);
}
#undef U8CONT

static inline size_t seq8len(const tro_u8code *seq)
{
	if (seq[0] == '\0')
		return 0;
	else if (seq[1] == '\0')
		return 1;
	else if (seq[2] == '\0')
		return 2;
	else if (seq[3] == '\0')
		return 3;
	else
		return 4;
}
static inline size_t u8len(const tro_u8code byte)
{
	/*
	   A máscara & (and) sempre pega um bit
	   menos significativo extra do cabeçalho,
	   pois precisamos garantir que o cabeçalho
	   é sequido de um bit 0, e então as comparações
	   fazem sentido.
	*/
	return (((byte & 0xF8) == 0xF0) * 4) + (((byte & 0xF0) == 0xE0) * 3) +
	       (((byte & 0xE0) == 0xC0) * 2) + (((byte & 0x80) == 0x00) * 1);
}
/*
   Macro para validar se um byte
   é uma continuação UTF-8 ou não
*/
#define IS_U8CONT(byte) (((byte) & 0xC0) == 0x80)
static inline size_t u8_validate(const tro_u8code *seq, size_t len)
{
	len = MIN(len, 4);
	size_t pos;
	for (pos = 1; pos < len; pos++) {
		if (!IS_U8CONT(seq[pos]))
			break;
	}
	return pos;
}
#undef IS_U8CONT

static inline tro_urune u8dasm_0(tro_u8code byte, size_t len)
{
	return ((len == 1) * byte)                    // Apenas retorna o byte.
	       + ((len == 2) * ((byte & 0x1F) << 6))  // Para o segundo layout.
	       + ((len == 3) * ((byte & 0x0F) << 12)) // Para o terceiro layout.
	       + ((len == 4) * ((byte & 0x07) << 18)); // Para o último layout.
}

static inline tro_urune u8dasm_1(tro_u8code byte, size_t len)
{
	// Se `len == 1`, só retorna `0x000000`.
	return ((len == 2) * (byte & 0x3F))          // Para o segundo layout.
	       + ((len == 3) * (byte & 0x3F) << 6)   // Para o terceiro layout.
	       + ((len == 4) * (byte & 0x3F) << 12); // Para o último layout.
}

static inline tro_urune u8dasm_2(tro_u8code byte, size_t len)
{
	// Se `len == 1` ou `len == 2`, só retorna `0x000000`.
	return ((len == 3) * (byte & 0x3F))           // Para o terceiro layout.
	       + ((len == 4) * ((byte & 0x3F) << 6)); // Para o último layout.
}

static inline tro_urune u8dasm_3(tro_u8code byte, size_t len)
{
	/*
	   Só retorna algo se precisar desmontar o byte
	   do último layout, retorna `0x000000` caso contrário.
	*/
	return (len == 4) * (byte & 0x3F);
}

static inline tro_u16code u16asm_w1(size_t len, tro_urune rune)
{
	return ((len == 1) * rune) // Apenas retorna o surrogate.
	       + ((len == 2) *
	          ((rune >> 10) | 0xD800)); // Monta o surrogate high.
}

static inline tro_u16code u16asm_w2(size_t len, tro_urune rune)
{
	// Monta o surrogate low, se precisar.
	return (len == 2) * ((rune & 0x3FF) | 0xDC00);
}
