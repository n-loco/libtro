#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code EARTH_GLOBE_AMERICAS[] = "🌎";
#define EARTH_GLOBE_AMERICAS_U 0x01F30E

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(EARTH_GLOBE_AMERICAS, &rune);

	bool success = read == 4 && rune == EARTH_GLOBE_AMERICAS_U;

	return !success;
}
