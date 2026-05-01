#include <test_utils.h>

#include <stddef.h>
#include <stdlib.h>

#include <tro/string.h>

static const char *EXP_STR = "8️⃣ 1️⃣6️⃣ 🧐";

int main(void)
{

	tro_strdybuf *buf = tro_create_strdybuf(64);

	tro_strdybuf_writes(buf, "8️⃣ ", 0);
	tro_strdybuf_writes16(buf, u"1️⃣6️⃣ ", 0);
	tro_strdybuf_writec(buf, U'🧐', 1);
	tro_strdybuf_writec(buf, U'🐁', 2);

	size_t strl;
	char *str = tro_strdybuf_getlloc(buf, &strl);

	tro_destroy_strdybuf(buf);

	str_assert("8️⃣ 1️⃣6️⃣ 🧐🐁🐁", EXP_STR, 35, str, strl);

	free(str);
}
