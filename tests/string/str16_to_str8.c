#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <uchar.h>

#include <tro/string.h>

static const char EXPECT_OK[]     = "Olá, mundo! 🌎";
static const char EXPECT_BROKEN[] = "Olá, mundo! ";

#define OK_CAP 18
#define BROKEN_CAP 17

static bool impl__str8_assert(const char *name, const char *e, size_t el,
                              const char *g, size_t gl);

#define str8_assert(name, e, el, g, gl)                                        \
	if (!impl__str8_assert(name, e, el, g, gl))                            \
		return 1;

int main(void)
{
	const char16_t src[] = u"Olá, mundo! 🌎";

	char *ok      = malloc(OK_CAP);
	size_t ok_len = tro_conv_str16_to_str(src, 0, ok, OK_CAP);
	str8_assert("Ok capacity", EXPECT_OK, 17, ok, ok_len);

	char *broken      = malloc(BROKEN_CAP);
	size_t broken_len = tro_conv_str16_to_str(src, 0, broken, BROKEN_CAP);
	str8_assert("Insufficient capacity", EXPECT_BROKEN, 13, broken,
	            broken_len);
}

static bool impl__str8_assert(const char *name, const char *e, size_t el,
                              const char *g, size_t gl)
{
	if (el != gl) {
		fprintf(stderr,
		        "On \"%s\", different len: expected %zu, got %zu", name,
		        el, gl);
		return false;
	}

	for (size_t i = 0; i < el; i++) {
		const char expected = e[i];
		const char got      = g[i];

		if (expected != got) {
			fprintf(stderr,
			        "On \"%s\", unmatched byte at position %zu\n",
			        name, i);

			fprintf(stderr, "  expected bytes:");
			for (size_t i = 0; i < el; i++) {
				fprintf(stderr, " \\x%02hhX", e[i]);
			}
			fprintf(stderr, "\n");

			fprintf(stderr, "       got bytes:");
			for (size_t i = 0; i < el; i++) {
				fprintf(stderr, " \\x%02hhX", g[i]);
			}
			fprintf(stderr, "\n");

			return false;
		}
	}

	return true;
}
