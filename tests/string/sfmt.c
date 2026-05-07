#include <test_utils.h>
#include <assert.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

#include <tro/string.h>
#include <tro/dybuffer.h>

#define GENSTRCAP 513
static char genstr[GENSTRCAP];

static size_t wrappbuf_sfmt(const char *format, ...);

int main(void)
{
	{
		const char XSTR[] = "Texto random 1\n";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl = wrappbuf_sfmt("Texto %s %d\n", "random", 1);
		str_assert("Texto random 1", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] = "[TRO] [   TRO] [TRO   ]";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl =
		    wrappbuf_sfmt("[%s] [%6s] [%-6s]", "TRO", "TRO", "TRO");
		str_assert("[TRO] [   TRO] [TRO   ]", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] = "Olá 8, Olá 16, 👋 32";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl =
		    wrappbuf_sfmt("%s 8, %S 16, %c 32", "Olá", u"Olá", U'👋');
		str_assert("Olá 8, Olá 16, 👋 32", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] = "[🔋     ] [     🔋] [🔋🔋🔋🔋🔋🔋]";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl =
		    wrappbuf_sfmt("[%-6c] [%6c] [%#6c]", U'🔋', U'🔋', U'🔋');
		str_assert("🔋", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] = "    32 000032 321608040201";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl =
		    wrappbuf_sfmt("%6d %06d %6llu", 32, 32, 321608040201ULL);
		str_assert("32 & 000032 & 321608040201", XSTR, XLEN, genstr,
		           strl);
	}

	{
		const char XSTR[] = " 1 -1 +1 -1  1 +1";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl = wrappbuf_sfmt("%2d %2d %+2d %+2d %2u %+2u", 1, -1,
		                            1, -1, 1, 1);
		str_assert(" 1 -1 +1 -1  1 +1", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] =
		    "0 0 0.0 0.000000 "
		    "   3.5 0003.5   3.50 003.50";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl = wrappbuf_sfmt(
		    "%#f %.f %.1f %f "
		    "%#6f %#06f %6.2f %06.2f",
		    0.0, 0.0, 0.0, 0.0, 3.5, 3.5, 3.5, 3.5);
		str_assert("0 & 3.5", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] = " 1 -1 +1 -1  0 -0 +0 -0";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl = wrappbuf_sfmt(
		    "%#2f %#2f %+#2f %+#2f "
		    "%#2f %#2f %+#2f %+#2f",
		    1.0, -1.0, 1.0, -1.0, 0.0, -0.0, 0.0, -0.0);
		str_assert(" 1 -1 +1 -1  0 -0 +0 -0", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] = "NaN NaN +NaN -NaN";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl =
		    wrappbuf_sfmt("%f %f %+f %+f", NAN, -NAN, NAN, -NAN);
		str_assert("NaN", XSTR, XLEN, genstr, strl);
	}

	{
		const char XSTR[] = "Infinity -Infinity +Infinity -Infinity";
		const size_t XLEN = sizeof(XSTR) - 1;

		size_t strl = wrappbuf_sfmt("%f %f %+f %+f", INFINITY,
		                            -INFINITY, INFINITY, -INFINITY);
		str_assert("Infinity", XSTR, XLEN, genstr, strl);
	}
}

#define dynbuf tro_strdybuf_dybuffer(buf)

static size_t wrappbuf_sfmt(const char *format, ...)
{
	tro_strdybuf *buf = tro_create_strdybuf(GENSTRCAP);
	assert(buf != NULL);

	va_list args;
	va_start(args, format);
	{
		bool success = tro_vsfmt(dynbuf, format, args);
		assert(success);
	}
	va_end(args);

	size_t len = tro_strdybuf_get(buf, genstr, GENSTRCAP);

	tro_destroy_strdybuf(buf);
	return len;
}
