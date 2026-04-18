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

#define c(s) (const tro_u16code *)u##s
#define rc(...)                                                                \
	(const tro_u16code[])                                                  \
	{                                                                      \
		__VA_ARGS__, 0x0000                                            \
	}

int main(void)
{
	tro_urune a_wacute;
	size_t a_wacute_r = tro_u16codes_to_urune(c("Á"), 0, &a_wacute);
	urune_assert("Á", U'Á', 1, a_wacute, a_wacute_r);

	tro_urune wave;
	size_t wave_r = tro_u16codes_to_urune(c("👋"), 0, &wave);
	urune_assert("👋", U'👋', 2, wave, wave_r);

	tro_urune cut_earth;
	size_t cut_earth_r = tro_u16codes_to_urune(rc(0xD83C), 0, &cut_earth);
	urune_assert("Cut '🌎'", U'�', 1, cut_earth, cut_earth_r);

	tro_urune broken_earth;
	size_t broken_earth_r =
	    tro_u16codes_to_urune(rc(0xD83C, 0xFFFF), 0, &broken_earth);
	urune_assert("Broken '🌎'", U'�', 1, broken_earth, broken_earth_r);
}
