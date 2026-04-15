#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <tro/string.h>

static bool impl__u8_assert(const char *name, const tro_u8code *eu8,
                            size_t eu8l, const tro_u8code *gu8, size_t gu8l);

#define u8_assert(name, eu8, eu8l, gu8, gu8l)                                  \
	if (!impl__u8_assert(name, eu8, eu8l, gu8, gu8l))                      \
		return 1;

static const tro_u8code EXPECT_A[TRO_MULTI_U8CODE_MAX]          = "A";
static const tro_u8code EXPECT_A_WACUTE[TRO_MULTI_U8CODE_MAX]   = "Á";
static const tro_u8code EXPECT_CJK[TRO_MULTI_U8CODE_MAX]        = "界";
static const tro_u8code EXPECT_EARTH_GLOBE[TRO_MULTI_U8CODE_MAX] = "🌎";

static const tro_u8code EXPECT_REPLACE_CHAR[TRO_MULTI_U8CODE_MAX] = "�";

#define U16_RESRVD 0x00D8F3
#define OUTOFRANGE 0xFFFFFF

int main(void)
{
	tro_u8code a[TRO_MULTI_U8CODE_MAX];
	size_t a_n = tro_urune_to_u8codes(U'A', a);
	u8_assert("A", EXPECT_A, 1, a, a_n);

	tro_u8code a_wacute[TRO_MULTI_U8CODE_MAX];
	size_t a_wacute_n = tro_urune_to_u8codes(U'Á', a_wacute);
	u8_assert("Á", EXPECT_A_WACUTE, 2, a_wacute, a_wacute_n);

	tro_u8code cjk[TRO_MULTI_U8CODE_MAX];
	size_t cjk_n = tro_urune_to_u8codes(U'界', cjk);
	u8_assert("界", EXPECT_CJK, 3, cjk, cjk_n);

	tro_u8code earth_globe[TRO_MULTI_U8CODE_MAX];
	size_t earth_globe_n = tro_urune_to_u8codes(U'🌎', earth_globe);
	u8_assert("🌎", EXPECT_EARTH_GLOBE, 4, earth_globe, earth_globe_n);

	tro_u8code u16_resrvd[TRO_MULTI_U8CODE_MAX];
	size_t u16_resrvd_n = tro_urune_to_u8codes(U16_RESRVD, u16_resrvd);
	u8_assert("UTF-16 reserved", EXPECT_REPLACE_CHAR, 3, u16_resrvd,
	          u16_resrvd_n);

	tro_u8code outofrage[TRO_MULTI_U8CODE_MAX];
	size_t outofrage_n = tro_urune_to_u8codes(OUTOFRANGE, outofrage);
	u8_assert("Out of range (> 0x10FFFF)", EXPECT_REPLACE_CHAR, 3,
	          outofrage, outofrage_n);
}

static bool impl__u8_assert(const char *name, const tro_u8code *eu8,
                            size_t eu8l, const tro_u8code *gu8, size_t gu8l)
{
	if (eu8l != gu8l) {
		fprintf(stderr,
		        "On \"%s\", different lens: expected %zu, "
		        "got %zu\n",
		        name, eu8l, gu8l);
		return false;
	}
	if ((eu8[0] != gu8[0]) || (eu8[1] != gu8[1]) || (eu8[2] != gu8[2]) ||
	    (eu8[3] != gu8[3])) {
		fprintf(stderr,
		        "On \"%s\", unmatched bytes: expected "
		        "\\x%02X\\x%02X\\x%02X\\x%02X, got "
		        "\\x%02X\\x%02X\\x%02X\\x%02X",
		        name, eu8[0], eu8[1], eu8[2], eu8[3], gu8[0], gu8[1],
		        gu8[2], gu8[3]);
		return false;
	}

	return true;
}
