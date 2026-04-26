#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <tro/string.h>

#define str_assert(name, es, el, gs, gl)                                       \
	{                                                                      \
		if ((size_t)el != (size_t)gl) {                                \
			fprintf(stderr,                                        \
			        "On \"%s\", unmatched lens: expected %zu, "    \
			        "got %zu\n",                                   \
			        name, (size_t)el, (size_t)gl);                 \
			return 1;                                              \
		}                                                              \
		if (strcmp(es, gs) != 0) {                                     \
			fprintf(stderr,                                        \
			        "On \"%s\", unmatched string: expected "       \
			        "\"%s\", got \"%s\"\n",                        \
			        name, es, gs);                                 \
			return 1;                                              \
		}                                                              \
	}

static const char *EXP_STR = "8️⃣ 1️⃣6️⃣ 🧐";

int main(void)
{

	tro_strdybuf *buf = tro_create_strdybuf(64);

	tro_strdybuf_writes(buf, "8️⃣ ", 0);
	tro_strdybuf_writes16(buf, u"1️⃣6️⃣ ", 0);
	tro_strdybuf_writec(buf, U'🧐');

	size_t strl;
	char *str = tro_strdybuf_getlloc(buf, &strl);

	tro_destroy_strdybuf(buf);

	str_assert("8️⃣ 1️⃣6️⃣ 🧐", EXP_STR, 27, str, strl);

	free(str);
}
