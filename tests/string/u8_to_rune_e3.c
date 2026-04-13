#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code BROKEN_EARTH_GLOBE_AMERICAS[] =
    "\xF0\x9F\xFF\x8E"; // O terceiro byte de '🌎' é inválido.

#define REPLACEMENT_CHARACTER 0x00FFFD

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(BROKEN_EARTH_GLOBE_AMERICAS, &rune);

	bool success = read == 2 && rune == REPLACEMENT_CHARACTER;

	return !success;
}
