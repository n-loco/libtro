#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <uchar.h>

#include <tro/string.h>

static const char16_t EXPECT_OK[]     = u"Olá, mundo! 🌎";
static const char16_t EXPECT_BROKEN[] = u"Olá, mundo! ";

#define OK_CAP 15
#define BROKEN_CAP 14

static bool impl__str16_assert(const char *name, const char16_t *e, size_t el,
                               const char16_t *g, size_t gl);

#define str16_assert(name, e, el, g, gl)                                       \
	if (!impl__str16_assert(name, e, el, g, gl))                           \
		return 1;

#define len16_only_assert(name, el, gl)                                        \
	{                                                                      \
		if (el != gl) {                                                \
			fprintf(stderr,                                        \
			        "On \"%s\", unmatched len: expected %zu, got " \
			        "%zu\n",                                       \
			        name, (size_t)el, (size_t)gl);                 \
			return 1;                                              \
		}                                                              \
	}

int main(void)
{
	const char src[] = "Olá, mundo! 🌎";

	char16_t *ok  = malloc(OK_CAP * sizeof(char16_t));
	size_t ok_len = tro_conv_str_to_str16(src, 0, ok, OK_CAP);
	str16_assert("Ok capacity", EXPECT_OK, 14, ok, ok_len);

	size_t ok_siz_test = tro_conv_str_to_str16(src, 0, NULL, 0);
	len16_only_assert("Ok size test", 14, ok_siz_test);

	char16_t *broken  = malloc(BROKEN_CAP * sizeof(char16_t));
	size_t broken_len = tro_conv_str_to_str16(src, 0, broken, BROKEN_CAP);
	str16_assert("Insufficient capacity", EXPECT_BROKEN, 12, broken,
	             broken_len);

	size_t nk_siz_test = tro_conv_str_to_str16(src, 0, broken, 0);
	len16_only_assert("Not ok size test", 0, nk_siz_test);
}

static bool impl__str16_assert(const char *name, const char16_t *e, size_t el,
                               const char16_t *g, size_t gl)
{
	if (el != gl) {
		fprintf(stderr,
		        "On \"%s\", different len: expected %zu, got %zu", name,
		        el, gl);
		return false;
	}

	for (size_t i = 0; i < el; i++) {
		const char16_t expected = e[i];
		const char16_t got      = g[i];

		if (expected != got) {
			fprintf(
			    stderr,
			    "On \"%s\", unmatched surrogate at position %zu\n",
			    name, i);

			fprintf(stderr, "  expected surrogates:");
			for (size_t i = 0; i < el; i++) {
				fprintf(stderr, " \\x%04X", e[i]);
			}
			fprintf(stderr, "\n");

			fprintf(stderr, "       got surrogates:");
			for (size_t i = 0; i < el; i++) {
				fprintf(stderr, " \\x%04X", g[i]);
			}
			fprintf(stderr, "\n");

			return false;
		}
	}

	return true;
}
