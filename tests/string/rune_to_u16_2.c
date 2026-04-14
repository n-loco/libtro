#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

#define EARTH_GLOBE_AMERICAS_W1 0xD83C
#define EARTH_GLOBE_AMERICAS_W2 0xDF0E

int main(void)
{
	tro_u16code ch[TRO_MULTI_U16CODE_MAX];
	size_t ch_l = tro_urune_to_u16codes(U'🌎', ch);

	bool expected_len        = ch_l == 2;
	bool expected_surrogates = (ch[0] == EARTH_GLOBE_AMERICAS_W1) &&
	                           (ch[1] == EARTH_GLOBE_AMERICAS_W2);

	bool success = expected_len && expected_surrogates;

	return !success;
}
