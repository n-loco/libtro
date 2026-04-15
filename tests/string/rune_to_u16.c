#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <tro/string.h>

static bool impl__u16_assert(const char *name, const tro_u16code *eu16,
                             size_t eu16l, const tro_u16code *gu16,
                             size_t gu16l);

#define u16_assert(name, eu16, eu16l, gu16, gu16l)                             \
	if (!impl__u16_assert(name, eu16, eu16l, gu16, gu16l))                 \
		return 1;

static const tro_u16code EXPECT_A_WACUTE[TRO_MULTI_U16CODE_MAX]    = u"Á";
static const tro_u16code EXPECT_EARTH_GLOBE[TRO_MULTI_U16CODE_MAX] = u"🌎";

static const tro_u16code EXPECT_REPLACE_CHAR[TRO_MULTI_U16CODE_MAX] = u"�";

#define U16_RESRVD 0x00D8F3
#define OUTOFRANGE 0xFFFFFF

int main(void)
{
	tro_u16code a_wacute[TRO_MULTI_U16CODE_MAX];
	size_t a_wacute_n = tro_urune_to_u16codes(U'Á', a_wacute);
	u16_assert("Á", EXPECT_A_WACUTE, 1, a_wacute, a_wacute_n);

	tro_u16code earth_globe[TRO_MULTI_U16CODE_MAX];
	size_t earth_globe_n = tro_urune_to_u16codes(U'🌎', earth_globe);
	u16_assert("🌎", EXPECT_EARTH_GLOBE, 2, earth_globe, earth_globe_n);

	tro_u16code u16_resrvd[TRO_MULTI_U16CODE_MAX];
	size_t u16_resrvd_n = tro_urune_to_u16codes(U16_RESRVD, u16_resrvd);
	u16_assert("UTF-16 reserved", EXPECT_REPLACE_CHAR, 1, u16_resrvd,
	           u16_resrvd_n);

	tro_u16code outofrage[TRO_MULTI_U16CODE_MAX];
	size_t outofrage_n = tro_urune_to_u16codes(OUTOFRANGE, outofrage);
	u16_assert("Out of range", EXPECT_REPLACE_CHAR, 1, outofrage,
	           outofrage_n);
}

static bool impl__u16_assert(const char *name, const tro_u16code *eu16,
                             size_t eu16l, const tro_u16code *gu16,
                             size_t gu16l)
{
	if (eu16l != gu16l) {
		fprintf(stderr,
		        "On \"%s\", different lens: expected %zu, got %zu\n",
		        name, eu16l, gu16l);
		return false;
	}

	if ((eu16[0] != gu16[0]) || (eu16[1] != gu16[1])) {
		fprintf(stderr,
		        "On \"%s\", unmatched surrogates: expected "
		        "\\x%04X\\x%04X, got \\x%04X\\x%04X",
		        name, eu16[0], eu16[1], gu16[0], gu16[1]);
		return false;
	}

	return true;
}
