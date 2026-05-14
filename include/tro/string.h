/** @file */

#ifndef TRO_STRING_H_
#define TRO_STRING_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

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
 * @param str é uma **string UTF-8** terminada em `'\0'`.
 *
 * @returns o comprimento de `str` em **runas** (**code points**).
 */
TRO__API size_t tro_strulen(const char *str);

/**
 * @param str é uma **string UTF-16** terminada em `'\0'`.
 *
 * @returns o comprimento de `str` em **runas** (**code points**).
 */
TRO__API size_t tro_str16ulen(const tro_char16 *str);

/**
 * Converte uma **string UTF-8** em uma **string UTF-16**.
 *
 * @param in é a **string UTF-8**.
 * @param inlen é um comprimento de `in`. Caso
 * seja `0`, se assume `in` terminado em `'\0'`.
 * @param out é onde a **string UTF-16** será escrita.
 * @param outcap é a capacidade de `out`.
 *
 * @returns o comprimento final da **string UTF-16**.
 */
TRO__API size_t tro_conv_str_to_str16(const char *in, size_t inlen,
                                      tro_char16 *out, size_t outcap);

/**
 * Converte uma **string UTF-16** em uma **string UTF-8**.
 *
 * @param in é a **string UTF-16**.
 * @param inlen é um comprimento de `in`. Caso
 * seja `0`, se assume `in` terminado em `'\0'`.
 * @param out é onde a **string UTF-8** será escrita.
 * @param outcap é a capacidade de `out`.
 *
 * @returns o comprimento final da **string UTF-8**.
 */
TRO__API size_t tro_conv_str16_to_str(const tro_char16 *in, size_t inlen,
                                      char *out, size_t outcap);

/**
 * Converte uma **string UTF-8** em uma **string UTF-16**.
 *
 * @param in é a **string UTF-8**.
 * @param inlen é um comprimento de `in`. Caso
 * seja `0`, se assume `in` terminado em `'\0'`.
 * @param lenout é um **ponteiro** para armazenar o
 * comprimento final da **string UTF-16**. Pode ser `NULL`.
 *
 * @returns a **string UTF-16** alocada via `malloc`, e
 * precisa ser liberada com `free`.
 */
TRO__API tro_char16 *tro_cnvlloc_str_to_str16(const char *in, size_t inlen,
                                              size_t *lenout);

/**
 * Converte uma **string UTF-16** em uma **string UTF-8**.
 *
 * @param in é a **string UTF-16**.
 * @param inlen é um comprimento de `in`. Caso
 * seja `0`, se assume `in` terminado em `'\0'`.
 * @param lenout é um **ponteiro** para armazenar o
 * comprimento final da **string UTF-8**. Pode ser `NULL`.
 *
 * @returns a **string UTF-8** alocada via `malloc`, e
 * precisa ser liberada com `free`.
 */
TRO__API char *tro_cnvlloc_str16_to_str(const tro_char16 *in, size_t inlen,
                                        size_t *lenout);

/**
 * Versão **UTF-16** do `strlen` padrão do C.
 *
 * @param str é uma **string UTF-16** terminada em `'\0'`.
 *
 * @returns o número de **surrogates** de `str`.
 */
TRO__API size_t tro_str16len(const tro_char16 *str);

/**
 * Versão **UTF-16** do `strnlen` padrão do C.
 *
 * @param str é uma **string UTF-16** terminada em `'\0'`.
 * @param maxlen é o comprimento **máximo** a ser testado.
 *
 * @returns o número de **surrogates** de `str` até `maxlen`.
 */
TRO__API size_t tro_str16nlen(const tro_char16 *str, size_t maxlen);

/**
 * Versão **UTF-16** do `strcmp` padrão do C.
 *
 * @param s1 é uma **string UTF-16** terminada em `'\0'`.
 * @param s2 é outra **string UTF-16** terminada em `'\0'`.
 *
 * @returns a diferença entre os primeiros **surrogates**
 * diferentes, `0` significa que `s1` e `s2` são iguais.
 */
TRO__API int tro_str16cmp(const tro_char16 *s1, const tro_char16 *s2);

/**
 * Versão **UTF-16** do `strncmp` padrão do C.
 *
 * @param s1 é uma **string UTF-16** terminada em `'\0'`.
 * @param s2 é outra **string UTF-16** terminada em `'\0'`.
 * @param maxlen é o comprimento **máximo** a ser testado.
 *
 * @returns a diferença entre os primeiros **surrogates**
 * diferentes, `0` significa que `s1` e `s2` são iguais até `maxlen`.
 */
TRO__API int tro_str16ncmp(const tro_char16 *s1, const tro_char16 *s2,
                           size_t maxlen);

/**
 * Um **buffer dinâmico** de **string UTF-8**
 * que implementa a **interface** `tro_dybuffer`
 *
 * @see tro_strdybuf_dybuffer
 */
typedef struct tro_strdybuf tro_strdybuf;
/**
 * **Tabela virtual** com os métodos de @ref tro_strdybuf
 * para a **interface** `tro_dybuffer`.
 */
TRO__API const tro_dybuffer_i *const tro_strdybuf_dybuffer_vt;
/**
 * @def tro_strdybuf_dybuffer(buf)
 *
 * Macro de **cast** de @ref tro_strdybuf para @ref tro_dybuffer_obj.
 */
#define tro_strdybuf_dybuffer(buf)                                             \
	((tro_dybuffer_obj){                                                   \
	    .obj    = (void *)buf,                                             \
	    .vtable = tro_strdybuf_dybuffer_vt,                                \
	})

/**
 * Cria um objeto @ref tro_strdybuf
 *
 * @param pagecap é o tamanho de cada página de memória.
 *
 * @returns o **ponteiro** para o objeto. `NULL` em caso de falha.
 *
 * @see tro_destroy_strdybuf
 */
TRO__API tro_strdybuf *tro_create_strdybuf(size_t pagecap);

/**
 * @param buf é o objeto a ser destruído.
 */
TRO__API void tro_destroy_strdybuf(tro_strdybuf *buf);

/**
 * @param buf o objeto.
 * @param data é uma **string UTF-8**.
 * @param datal é o comprimento de `data`,
 * caso seja `0`, se assume `data` terminado
 * em `'\0'`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_strdybuf_writes(tro_strdybuf *buf, const char *data,
                                  size_t datal);

/**
 * @param buf o objeto.
 * @param data é uma **string UTF-16**.
 * @param datal é o comprimento de `data`,
 * caso seja `0`, se assume `data` terminado
 * em `'\0'`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_strdybuf_writes16(tro_strdybuf *buf, const tro_char16 *data,
                                    size_t datal);

/**
 * @param buf o objeto.
 * @param data é uma **sequência de bytes arbitrária**.
 * @param datal é o comprimento de `data`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_strdybuf_writeb(tro_strdybuf *buf, const uint8_t *data,
                                  size_t datal);

/**
 * @param buf o objeto.
 * @param rune é uma **runa** (**code point**).
 * @param count diz quantas vezes se deve escrever `rune`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_strdybuf_writec(tro_strdybuf *buf, tro_urune rune,
                                  size_t count);

/**
 * @param buf o objeto.
 *
 * @returns a *preferência de encoding*.
 */
TRO__API tro_dybuf_pref tro_strdybuf_preference(const tro_strdybuf *buf);

/**
 * @param buf o objeto.
 * @param out é onde será escrita a *string* formada. Pode ser `NULL`.
 * @param outcap é a capacidade de `out` (ignorado se `out` for `NULL`).
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_strdybuf_get(tro_strdybuf *buf, char *out, size_t outcap);

/**
 * @param buf o objeto.
 * @param outlen é um **ponteiro** para armazenar
 * o comprimento final da *string*
 *
 * @returns a *string* formada alocada via `malloc`,
 * e precisa ser liberada com `free`.
 */
TRO__API char *tro_strdybuf_getlloc(tro_strdybuf *buf, size_t *outlen);

/**
 * Um **buffer dinâmico** de **string UTF-16**
 * que implementa a **interface** `tro_dybuffer`
 *
 * @see tro_str16dybuf_dybuffer
 */
typedef struct tro_str16dybuf tro_str16dybuf;
/**
 * **Tabela virtual** com os métodos de @ref tro_str16dybuf
 * para a **interface** `tro_dybuffer`.
 */
TRO__API const tro_dybuffer_i *const tro_str16dybuf_dybuffer_vt;
/**
 * @def tro_str16dybuf_dybuffer(buf)
 *
 * Macro de **cast** de @ref tro_str16dybuf para @ref tro_dybuffer_obj.
 */
#define tro_str16dybuf_dybuffer(buf)                                           \
	((tro_dybuffer_obj){                                                   \
	    .obj    = (void *)buf,                                             \
	    .vtable = tro_str16dybuf_dybuffer_vt,                              \
	})

/**
 * Cria um objeto @ref tro_str16dybuf
 *
 * @param pagecap é o tamanho de cada página de memória.
 *
 * @returns o **ponteiro** para o objeto. `NULL` em caso de falha.
 *
 * @see tro_destroy_str16dybuf
 */
TRO__API tro_str16dybuf *tro_create_str16dybuf(size_t pagecap);

/**
 * @param buf é o objeto a ser destruído.
 */
TRO__API void tro_destroy_str16dybuf(tro_str16dybuf *buf);

/**
 * @param buf o objeto.
 * @param data é uma **string UTF-8**.
 * @param datal é o comprimento de `data`,
 * caso seja `0`, se assume `data` terminado
 * em `'\0'`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_str16dybuf_writes(tro_str16dybuf *buf, const char *data,
                                    size_t datal);

/**
 * @param buf o objeto.
 * @param data é uma **string UTF-16**.
 * @param datal é o comprimento de `data`,
 * caso seja `0`, se assume `data` terminado
 * em `'\0'`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_str16dybuf_writes16(tro_str16dybuf *buf,
                                      const tro_char16 *data, size_t datal);

/**
 * @param buf o objeto.
 * @param data é uma **sequência de bytes arbitrária**.
 * @param datal é o comprimento de `data`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_str16dybuf_writeb(tro_str16dybuf *buf, const uint8_t *data,
                                    size_t datal);

/**
 * @param buf o objeto.
 * @param rune é uma **runa** (**code point**).
 * @param count diz quantas vezes se deve escrever `rune`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_str16dybuf_writec(tro_str16dybuf *buf, tro_urune rune,
                                    size_t count);

/**
 * @param buf o objeto.
 *
 * @returns a *preferência de encoding*.
 */
TRO__API tro_dybuf_pref tro_str16dybuf_preference(const tro_str16dybuf *buf);

/**
 * @param buf o objeto.
 * @param out é onde será escrita a *string* formada. Pode ser `NULL`.
 * @param outcap é a capacidade de `out` (ignorado se `out` for `NULL`).
 *
 * @returns o comprimento final da *string*.
 */
TRO__API size_t tro_str16dybuf_get(tro_str16dybuf *buf, tro_char16 *out,
                                   size_t outcap);

/**
 * @param buf o objeto.
 * @param outlen é um **ponteiro** para armazenar
 * o comprimento final da *string*
 *
 * @returns a *string* formada alocada via `malloc`,
 * e precisa ser liberada com `free`.
 */
TRO__API tro_char16 *tro_str16dybuf_getlloc(tro_str16dybuf *buf,
                                            size_t *outlen);

/**
 * @page tro_sfmt_spec Especificadores de Formato TRO
 *
 * # Visão Geral
 * Os especificadores de formato são _**muito
 * inspirados**_ nos *especificadores de formato
 * padrões do C*. Porém algumas adições, remoções
 * e modificações foram feitas, então **TODOS**
 * os especificadores de formatos estão devidamente
 * documentados aqui!
 *
 * @note
 * Vale mencionar que nos exemplos daqui,
 * é utilizada uma **função fictícia**
 * com a assinatura:
 * ```c
 * char *fmt(const char *format, ...);
 * ```
 * Para **representar** *qualquer função* que siga
 * este padrão de especificação de formato.
 *
 * # Especificadores
 * ## Caracteres
 * O *único* especificador de formato é:
 * - `%%c` que espera um @ref tro_urune.
 *
 * É possível usar as flags `-` e `#` (*não ao mesmo tempo*),
 * sendo `-` para ajustar o lado de justificação e `#` para repetir o caractere,
 * devem estar em conjunto com a largura do campo e funciona assim:
 *
 * _**Formatação**_
 * ```c
 * fmt("[%c] [%4c] [%-4c] [%#4c]", U'🔋', U'🔋', U'🔋', U'🔋');
 * ```
 * _**Resultado**_
 * ```c
 * "[🔋] [   🔋] [🔋   ] [🔋🔋🔋🔋]"
 * ```
 *
 * Embora `%%c` espere um @ref tro_urune e o exemplo
 * acima usou um *literal de caractere de 32-bits*
 * você pode tranquilamente usar o *literal de caractere*
 * comum e o *literal de caractere de 16-bits*:
 *
 * ```c
 * // Ambos erfeitamente possíveis.
 * fmt("%c", 'A'); // Literal comum.
 * fmt("%c", u'界'); // Literal de 16 bits.
 * ```
 *
 * Inclusive, variáveis que não **são do tipo** @ref tro_urune
 * ainda sim **são promovidos** quando passados como argumentos:
 *
 * ```c
 * char ascii_a = 'a'; // Tipo menor que tro_urune.
 * fmt("%c", ascii_a); // Mesmo assim funciona.
 * ```
 *
 * ## Strings
 * Para formatação de *strings*, existem dois especificadores:
 * - `%%s` para `const char *` (**UTF-8**);
 * - `%%S` para `const tro_char16 *` (**UTF-16**).
 *
 * É possível usar a *flag* `-` em conjunto com
 * a largura do campo para criar coisas assim:
 *
 * _**Formatação**_
 * ```c
 * fmt("[%s] [%6s] [%-6s]", "TRO", "TRO", "TRO");
 * ```
 * _**Resultado**_
 * ```c
 * "[TRO] [   TRO] [TRO   ]"
 * ```
 *
 * ## Números Inteiros
 * Para formatação de *números inteiros*, há
 * os seguintes especificadores:
 * - `%%d` ou `%%i` para *decimal de inteiro com sinal*;
 * - `%%u` para *decimal de inteiro sem sinal*;
 * - `%%x` para *hexadecimal*;
 * - `%%X` também para *hexiadecimal*, porém com as letras *maiúsculas*;
 * - `%%o` para *octal*.
 *
 * Tabela de **modificadores de tamanho** em *ordem crescente*:
 * |Modificador|Spec `d`/`i`                    |Spec `u`/`x`/`X`/`o` |
 * |-----------|--------------------------------|--------------------------------|
 * |`hh`       |`signed char`                   |`unsigned char` |
 * |`h`        |`short` (`signed short`)        |`unsigned short` |
 * |*nenhum*   |`int` (`signed int`)            |`unsigned int` |
 * |`l`        |`long` (`signed long`)          |`unsigned long` |
 * |`ll`       |`long long` (`signed long long`)|`unsigned long long` |
 * |`j`        |`intmax_t`                      |`uintmax_t` |
 * |`z`        |*versão signed do* `size_t`     |`size_t` |
 * |`t`        |`ptrdiff_t`                     |*versão unsigned do*
 * `ptrdiff_t`| E assim se utiliza:
 *
 * _**Formatação**_
 * ```c
 * int d = 35;
 * unsigned long long llu = 321608040201ULL;
 * size_t zu = 17;
 * char hhX = -1;
 *
 * fmt("%d %llu %zu %hhX", d, llu, zu, hhX);
 * ```
 * _**Resultado**_
 * ```c
 * "35 321608040201 17 FF"
 * ```
 *
 * Existem as flags, `+`, `-` e `0` junto com com a largura do
 * campo para personalizar a formatação dos números, sendo
 * `+` para o número **sempre** apresentar sinal, `-` para
 * para ajustar o lado de justificação e `0` para preencher
 * os espaços em branco com `'0'` no lugar de `' '`:
 *
 * _**Formatação**_
 * ```c
 * fmt("[%+60] [%3d] [%-3d] [%03d]", 60, 60, 60, 60);
 * ```
 * _**Resultado**_
 * ```c
 * "[+62] [ 62] [62 ] [062]"
 * ```
 *
 * Ou:
 *
 * _**Formatação**_
 * ```c
 * size_t n = 65535;
 *
 * fmt("%16zX %016zX", n, n);
 * ```
 * _**Resultado**_
 * ```c
 * "        FFFFFFFF 00000000FFFFFFFF"
 * ```
 *
 * ## Números de Ponto Flutuante
 * Para formatação de *números de ponto
 * flutuante*, há os seguintes especificadores:
 * - `%%f` para *número real*;
 * - `%%e` para *número em notação E*;
 * - `%%E` para *número em notação E em maiúsculo*;
 * - `%%a` *ainda não documentado*;
 * - `%%A` *ainda não documentado*;
 * - `%%g` *ainda não documentado*;
 * - `%%G` *ainda não documentado*.
 *
 * O *único* **modificador de tamanho** é `L`, sendo assim:
 * - *nenhum* para `float` e `double`;
 * - `L` para `long double`.
 *
 * Também é possível especificar a *precisão*
 * em casas decimais de fração usando `.` seguido
 * por um número.
 *
 * As *flags* disponíveis são `+`, `-`, `#` e `0`, sendo:
 * - `+` para o número *sempre* apresentar sinal (e apresentar o sinal de
 * `NAN`);
 * - `-` para ajustar o lado de justificação;
 * - `0` para preencher os espaços em branco com `'0'` no lugar de `' '`;
 * - `#` para remover *zeros extras à direita*.
 *
 * `-` e `0` trabalham em conjunto com a largura do campo.
 *
 * Utiliza-se assim:
 *
 * _**Formatações**_
 * ```c
 * // 1.
 * fmt("[%f] [%.2f] [%012f] [%-12f] [%06.2f]
 * [%-6.2f]", 3.5, 3.5, 3.5, 3.5, 3.5, 3.5);
 * // 2.
 * fmt("[%#f] [%#.2f] [%#8f] [%-#8f] [%#8.2f] [%0#8f]
 * [%0#8.2f]", 3.5, 3.5, 3.5, 3.5, 3.5, 3.5, 3.5);
 * // 3.
 * fmt("%#f %#f %+#f %#f %#f %+#f %+#f", -0.0, 0.0, 0.0, NAN, -NAN, NAN, -NAN);
 * ```
 * _**Resultados**_
 * ```c
 * // 1.
 * "[3.500000] [3.50] [00003.500000] [3.500000    ] [003.50] [3.50  ]"
 * // 2.
 * "[3.5] [3.5] [     3.5] [     3.5] [3.5     ] [000003.5] [000003.5]"
 * // 3.
 * "-0 0 +0 NaN NaN +NaN -NaN"
 * ```
 *
 * # Appendice
 * ## Largura do Campo & Precisão
 * No lugar de um número, você pode usar um `*`, e a
 * função usará um `unsigend int` passado como *argumento*
 * para determinar o número da largura ou da precisão:
 *
 * _**Formatações**_
 * ```c
 * // 1.
 * fmt("[%*s]", "TRO", 8);
 * // 2.
 * fmt("[%.*f]", 4.3, 2);
 * ```
 * _**Resultados**_
 * ```c
 * // 1.
 * "[     TRO]"
 * // 2.
 * "[4.30]"
 * ```
 *
 * ## Erro
 * Quando uma função **falha em formatar** algo
 * ela **não** vai indicar **nenhum erro em retorno**,
 * em vez disso, ela **substitui** a parte desconhecida
 * com o caractere `u'␀'` e segue normalmente:
 *
 * _**Formatação**_
 * ```c
 * fmt("teste %llv %d", 3);
 * ```
 * _**Resultado**_
 * ```c
 * "teste ␀ 3"
 * ```
 */

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param buf é o *buffer* de **destino**.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_sfmt(tro_dybuffer_obj buf, const char *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_sfmt.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param buf é o *buffer* de **destino**.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_vsfmt(tro_dybuffer_obj buf, const char *format, va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param buf é o *buffer* de **destino**.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_sfmt16(tro_dybuffer_obj buf, const tro_char16 *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_sfmt16.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param buf é o *buffer* de **destino**.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns `true` se sucedido, `false` se falhou.
 */
TRO__API bool tro_vsfmt16(tro_dybuffer_obj buf, const tro_char16 *format,
                          va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns uma **string UTF-8** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API char *tro_str_fmt(size_t *outlen, const char *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_str_fmt.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns uma **string UTF-8** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API char *tro_vstr_fmt(size_t *outlen, const char *format, va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns uma **string UTF-8** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API char *tro_str_fmt16(size_t *outlen, const tro_char16 *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_str_fmt16.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns uma **string UTF-8** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API char *tro_vstr_fmt16(size_t *outlen, const tro_char16 *format,
                              va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns uma **string UTF-16** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API tro_char16 *tro_str16_fmt(size_t *outlen, const char *format, ...);

/**
 * Implementação com **lista variádica** de @ref tro_str16_fmt.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-8**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns uma **string UTF-16** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API tro_char16 *tro_vstr16_fmt(size_t *outlen, const char *format,
                                    va_list args);

/**
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param ... argumentos baseados em `format`.
 *
 * @returns uma **string UTF-16** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API tro_char16 *tro_str16_fmt16(size_t *outlen, const tro_char16 *format,
                                     ...);

/**
 * Implementação com **lista variádica** de @ref tro_str16_fmt16.
 *
 * Esta função segue o padrão @ref tro_sfmt_spec.
 *
 * @param outlen é onde será armazenado o comprimento final
 * da *string*. Pode ser `NULL`.
 * @param format é a **string de formatação** em **UTF-16**.
 * @param args **lista variádica** de argumentos baseados em `format`.
 *
 * @returns uma **string UTF-16** alocada via `malloc` que deve
 * ser liberada com `free`, `NULL` em caso de falha.
 */
TRO__API tro_char16 *tro_vstr16_fmt16(size_t *outlen, const tro_char16 *format,
                                      va_list args);

TRO__C_API_END

#endif // TRO_STRING_H_
