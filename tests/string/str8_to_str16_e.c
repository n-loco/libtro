#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uchar.h>

#include <tro/string.h>

static const char HELLO_WORLD[]                = "Olá, mundo! 🌎";
static const char16_t BROKEN_HELLO_WORLD_U16[] = u"Olá, mundo! �";

#define HELLO_WORLD_U16_LEN 13

#include <stdio.h>

int main(void)
{
	const size_t str_mem = 14;
	const size_t str_cap = 14;

	char16_t *str  = malloc(str_mem);
	size_t str_len = tro_conv_str_to_str16(HELLO_WORLD, 0, str, str_cap);

	bool same_len = str_len == HELLO_WORLD_U16_LEN;
	bool str_eq   = same_len;
	if (str_eq) {
		for (size_t i = 0; i < str_len; i++) {
			const char16_t expected = BROKEN_HELLO_WORLD_U16[i];
			const char16_t got      = str[i];
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
