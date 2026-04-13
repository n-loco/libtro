#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code CUT_CJK_IDEOGRAPH[] =
    "\xE7\x95"; // Falta o último byte de '界'.

#define REPLACEMENT_CHARACTER 0x00FFFD

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(CUT_CJK_IDEOGRAPH, &rune);

	bool success = read == 2 && rune == REPLACEMENT_CHARACTER;

	return !success;
}
