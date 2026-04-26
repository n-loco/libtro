#include <stddef.h>

#include <tro/strconv.h>

#include "utils.h"

#define GENCAP TRO_INT_CHAR_MAX
static char genbuf[GENCAP];

static const char N0[] = "0";

static const char N1[]    = "1";
static const char N345[]  = "345";
static const char N1000[] = "1000";

static const char NN1[]    = "-1";
static const char NN345[]  = "-345";
static const char NN1000[] = "-1000";

int main(void)
{
	size_t u0l = tro_uint2str(0, genbuf, GENCAP);
	str_assert("0", N0, 1, genbuf, u0l);

	size_t u1l = tro_uint2str(1, genbuf, GENCAP);
	str_assert("1", N1, 1, genbuf, u1l);

	size_t u345l = tro_uint2str(345, genbuf, GENCAP);
	str_assert("345", N345, 3, genbuf, u345l);

	size_t u1000l = tro_uint2str(1000, genbuf, GENCAP);
	str_assert("1000", N1000, 4, genbuf, u1000l);

	size_t u84532l = tro_uint2str(84532, NULL, 0);
	len_assert("84532 len test", 5, u84532l);

	size_t u0l2 = tro_uint2str(0, NULL, 0);
	len_assert("0 len test", 1, u0l2);

	size_t i0l = tro_int2str(0, genbuf, GENCAP);
	str_assert("(signed) 0", N0, 1, genbuf, i0l);

	size_t i1l = tro_int2str(1, genbuf, GENCAP);
	str_assert("(signed) 1", N1, 1, genbuf, i1l);

	size_t in1l = tro_int2str(-1, genbuf, GENCAP);
	str_assert("-1", NN1, 2, genbuf, in1l);

	size_t i345l = tro_int2str(345, genbuf, GENCAP);
	str_assert("(signed) 345", N345, 3, genbuf, i345l);

	size_t in345l = tro_int2str(-345, genbuf, GENCAP);
	str_assert("-345", NN345, 4, genbuf, in345l);

	size_t i1000l = tro_int2str(1000, genbuf, GENCAP);
	str_assert("(signed) 1000", N1000, 4, genbuf, i1000l);

	size_t in1000l = tro_int2str(-1000, genbuf, GENCAP);
	str_assert("-1000", NN1000, 5, genbuf, in1000l);

	size_t i84532l = tro_int2str(84532, NULL, 0);
	len_assert("(signed) 84532 len test", 5, i84532l);

	size_t in84532l = tro_int2str(-84532, NULL, 0);
	len_assert("-84532 len test", 6, in84532l);

	size_t i0l2 = tro_int2str(0, NULL, 0);
	len_assert("(signed) 0 len test", 1, i0l2);

	size_t i1l2 = tro_int2str(1, NULL, 0);
	len_assert("(signed) 1 len test", 1, i1l2);

	size_t in1l2 = tro_int2str(-1, NULL, 0);
	len_assert("-1 len test", 2, in1l2);
}
