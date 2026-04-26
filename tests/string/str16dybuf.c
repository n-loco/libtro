#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <uchar.h>
#include <stdlib.h>
#include <stdio.h>

#include <tro/string.h>

static inline bool impl__str16_assert(const char *name, const char16_t *es,
                                      size_t el, const char16_t *gs, size_t gl);

#define str16_assert(name, es, el, gs, gl)                                     \
	if (!impl__str16_assert(name, es, el, gs, gl))                         \
		return 1;

static const char16_t *EXP_STR = u"8️⃣ 1️⃣6️⃣ 🧐";

int main(void)
{

	tro_str16dybuf *buf = tro_create_str16dybuf(64);

	tro_str16dybuf_writes(buf, "8️⃣ ", 0);
	tro_str16dybuf_writes16(buf, u"1️⃣6️⃣ ", 0);
	tro_str16dybuf_writec(buf, U'🧐');

	size_t strl;
	char16_t *str = tro_str16dybuf_getlloc(buf, &strl);

	tro_destroy_str16dybuf(buf);

	str16_assert("8️⃣ 1️⃣6️⃣ 🧐", EXP_STR, 13, str, strl);

	free(str);
}

static inline bool impl__str16_assert(const char *name, const char16_t *es,
                                      size_t el, const char16_t *gs, size_t gl)
{
	if (el != gl) {
		fprintf(stderr,
		        "On \"%s\", unmatched lens: expected %zu, "
		        "got %zu\n",
		        name, el, gl);
		return false;
	}

	if (tro_str16cmp(es, gs) != 0) {
		char *u8es = tro_cnvlloc_str16_to_str(es, el, NULL);
		char *u8gs = tro_cnvlloc_str16_to_str(gs, gl, NULL);

		fprintf(stderr,
		        "On \"%s\", unmatched string: expected "
		        "\"%s\", got \"%s\"\n",
		        name, u8es, u8gs);

		free(u8gs);
		free(u8es);
		return false;
	}

	return true;
}
