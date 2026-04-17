#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <tro/string.h>

#define urune_assert(name, e, er, g, gr)                                       \
	{                                                                      \
		if (er != gr) {                                                \
			fprintf(stderr, "On \"%s\": expected %zu, got %zu\n",  \
			        name, (size_t)er, (size_t)gr);                 \
			return 1;                                              \
		}                                                              \
		if (e != g) {                                                  \
			fprintf(stderr,                                        \
			        "On \"%s\": expected \\x%06X, got \\x%06X",    \
			        name, e, g);                                   \
			return 1;                                              \
		}                                                              \
	}

#define c(s) (const tro_u8code *)s

int main(void)
{
	tro_urune a;
	size_t a_r = tro_u8codes_to_urune(c("A"), 0, &a);
	urune_assert("A", U'A', 1, a, a_r);

	tro_urune a_wacute;
	size_t a_wacute_r = tro_u8codes_to_urune(c("Á"), 0, &a_wacute);
	urune_assert("Á", U'Á', 2, a_wacute, a_wacute_r);

	tro_urune cjk;
	size_t cjk_r = tro_u8codes_to_urune(c("界"), 0, &cjk);
	urune_assert("界", U'界', 3, cjk, cjk_r);

	tro_urune earth_globe;
	size_t earth_globe_r = tro_u8codes_to_urune(c("🌎"), 0, &earth_globe);
	urune_assert("🌎", U'🌎', 4, earth_globe, earth_globe_r);

	tro_urune illgl_byte1;
	size_t illgl_byte1_r =
	    tro_u8codes_to_urune(c("\xBF\x8F"), 0, &illgl_byte1);
	urune_assert("Illegal byte 1", U'�', 1, illgl_byte1, illgl_byte1_r);

	tro_urune illgl_byte2;
	size_t illgl_byte2_r =
	    tro_u8codes_to_urune(c("\xFF\x8F"), 0, &illgl_byte2);
	urune_assert("Illegal byte 2", U'�', 1, illgl_byte2, illgl_byte2_r);

	tro_urune cut_cjk;
	size_t cut_cjk_r = tro_u8codes_to_urune(c("\xE7\x95"), 0, &cut_cjk);
	urune_assert("Cut '界'", U'�', 2, cut_cjk, cut_cjk_r);

	tro_urune broken_earth;
	size_t broken_earth_r =
	    tro_u8codes_to_urune(c("\xF0\x9F\xFF\x8E"), 0, &broken_earth);
	urune_assert("Broken '🌎'", U'�', 2, broken_earth, broken_earth_r);

	tro_urune u16_reserved;
	size_t u16_reserved_r =
	    tro_u8codes_to_urune(c("\xED\xA3\xBF"), 0, &u16_reserved);
	urune_assert("UTF-16 reserved", U'�', 3, u16_reserved, u16_reserved_r);

	tro_urune overlong_a;
	size_t overlong_a_r =
	    tro_u8codes_to_urune(c("\xF0\x80\x81\x81"), 0, &overlong_a);
	urune_assert("Overlong A", U'�', 4, overlong_a, overlong_a_r);
}
