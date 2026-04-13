#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code RESERVED_U16[] = "\xED\xA3\xBF";

#define REPLACEMENT_CHARACTER 0x00FFFD

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(RESERVED_U16, &rune);

	bool success = read == 3 && rune == REPLACEMENT_CHARACTER;

	return !success;
}
