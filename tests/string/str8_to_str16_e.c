#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <tro/string.h>

static const char HELLO_WORLD[] = "Olá, mundo! 🌎";
// clang-format off
static const tro_char16 BROKEN_HELLO_WORLD_U16[] = {
	0x004F, // O
	0x006C, // l
	0x00E1, // á
	0x002C, // ,
	0x0020, //  
	0x006D, // m
	0x0075, // u
	0x006E, // n
	0x0064, // d
	0x006F, // o
	0x0021, // !
	0x0020, //  
	0xFFFD, // � (que deve vir da impossibilidade de codificar 🌎.)
	0x0000, // Terminação da string.
};
// clang-format on

#define HELLO_WORLD_U16_LEN 13

#include <stdio.h>

int main(void)
{
	const size_t str_mem = 14;
	const size_t str_cap = 14;

	tro_char16 *str = malloc(str_mem);
	size_t str_len  = tro_conv_str_to_str16(HELLO_WORLD, 0, str, str_cap);

	bool same_len = str_len == HELLO_WORLD_U16_LEN;
	bool str_eq   = same_len;
	if (str_eq) {
		for (size_t i = 0; i < str_len; i++) {
			const tro_char16 expected = BROKEN_HELLO_WORLD_U16[i];
			const tro_char16 got      = str[i];
			if (expected != got) {
				fprintf(stderr,
				        "unmatched surrogates: expected "
				        "0x%04X, got 0x%04x\n",
				        expected, got);
				str_eq = false;
			}
		}
	} else
		fprintf(stderr, "not same len: expected %zu, got %zu\n",
		        (size_t)HELLO_WORLD_U16_LEN, str_len);

	free(str);

	return !str_eq;
}
