#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code OVERLONG_LATIN_CAPITAL_LETTER_A[] = "\xF0\x80\x81\x81";

#define REPLACEMENT_CHARACTER 0x00FFFD

int main(void)
{
	tro_urune rune;
	size_t read =
	    tro_u8codes_to_urune(OVERLONG_LATIN_CAPITAL_LETTER_A, &rune);

	bool success = read == 4 && rune == REPLACEMENT_CHARACTER;

	return !success;
}
