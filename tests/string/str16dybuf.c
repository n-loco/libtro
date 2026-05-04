#include <test_utils.h>

#include <stddef.h>
#include <stdbool.h>
#include <uchar.h>
#include <stdlib.h>

#include <tro/string.h>

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
