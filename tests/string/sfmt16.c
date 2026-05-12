#include <test_utils.h>
#include <assert.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

#include <tro/string.h>
#include <tro/uchar.h>
#include <tro/dybuffer.h>

#define GENSTRCAP 513
static tro_char16 genstr[GENSTRCAP];

static size_t wrappbuf_sfmt16(const tro_char16 *format, ...);

int main(void)
{
	{
		const tro_char16 XSTR[] = u"Texto random 1\n";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl = wrappbuf_sfmt16(u"Texto %s %d\n", "random", 1);
		str16_assert("Texto random 1", XSTR, XLEN, genstr, strl);
	}

	{
		const tro_char16 XSTR[] = u"[TRO] [   TRO] [TRO   ]";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl =
		    wrappbuf_sfmt16(u"[%s] [%6s] [%-6s]", "TRO", "TRO", "TRO");
		str16_assert("[TRO] [   TRO] [TRO   ]", XSTR, XLEN, genstr,
		             strl);
	}

	{
		const tro_char16 XSTR[] = u"Olá 8, Olá 16, 👋 32";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl = wrappbuf_sfmt16(u"%s 8, %S 16, %c 32", "Olá",
		                              u"Olá", U'👋');
		str16_assert("Olá 8, Olá 16, 👋 32", XSTR, XLEN, genstr, strl);
	}

	{
		const tro_char16 XSTR[] = u"[🔋     ] [     🔋] [🔋🔋🔋🔋🔋🔋]";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl = wrappbuf_sfmt16(u"[%-6c] [%6c] [%#6c]", U'🔋',
		                              U'🔋', U'🔋');
		str16_assert("🔋", XSTR, XLEN, genstr, strl);
	}

	{
		const tro_char16 XSTR[] = u"    32 000032 321608040201";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl =
		    wrappbuf_sfmt16(u"%6d %06d %6llu", 32, 32, 321608040201ULL);
		str16_assert("32 & 000032 & 321608040201", XSTR, XLEN, genstr,
		             strl);
	}

	{
		const tro_char16 XSTR[] = u" 1 -1 +1 -1  1 +1";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl = wrappbuf_sfmt16(u"%2d %2d %+2d %+2d %2u %+2u", 1,
		                              -1, 1, -1, 1, 1);
		str16_assert(" 1 -1 +1 -1  1 +1", XSTR, XLEN, genstr, strl);
	}

	{
		const tro_char16 XSTR[] =
		    u"0 0 0.0 0.000000 "
		    "   3.5 0003.5   3.50 003.50";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl = wrappbuf_sfmt16(
		    u"%#f %.f %.1f %f "
		    "%#6f %#06f %6.2f %06.2f",
		    0.0, 0.0, 0.0, 0.0, 3.5, 3.5, 3.5, 3.5);
		str16_assert("0 & 3.5", XSTR, XLEN, genstr, strl);
	}

	{
		const tro_char16 XSTR[] = u" 1 -1 +1 -1  0 -0 +0 -0";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl = wrappbuf_sfmt16(
		    u"%#2f %#2f %+#2f %+#2f "
		    "%#2f %#2f %+#2f %+#2f",
		    1.0, -1.0, 1.0, -1.0, 0.0, -0.0, 0.0, -0.0);
		str16_assert(" 1 -1 +1 -1  0 -0 +0 -0", XSTR, XLEN, genstr,
		             strl);
	}

	{
		const tro_char16 XSTR[] = u"NaN NaN +NaN -NaN";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl =
		    wrappbuf_sfmt16(u"%f %f %+f %+f", NAN, -NAN, NAN, -NAN);
		str16_assert("NaN", XSTR, XLEN, genstr, strl);
	}

	{
		const tro_char16 XSTR[] =
		    u"Infinity -Infinity +Infinity -Infinity";
		const size_t XLEN = (sizeof(XSTR) / sizeof(tro_char16)) - 1;

		size_t strl = wrappbuf_sfmt16(u"%f %f %+f %+f", INFINITY,
		                              -INFINITY, INFINITY, -INFINITY);
		str16_assert("Infinity", XSTR, XLEN, genstr, strl);
	}
}

#define dynbuf tro_str16dybuf_dybuffer(buf)

static size_t wrappbuf_sfmt16(const tro_char16 *format, ...)
{
	tro_str16dybuf *buf = tro_create_str16dybuf(GENSTRCAP);
	assert(buf != NULL);

	va_list args;
	va_start(args, format);
	{
		bool success = tro_vsfmt16(dynbuf, format, args);
		assert(success);
	}
	va_end(args);

	size_t len = tro_str16dybuf_get(buf, genstr, GENSTRCAP);

	tro_destroy_str16dybuf(buf);
	return len;
}
