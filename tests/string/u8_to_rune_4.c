#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code EARTH_GLOBE_AMERICAS[] = "🌎";

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(EARTH_GLOBE_AMERICAS, &rune);

	bool success = read == 4 && rune == U'🌎';

	return !success;
}
