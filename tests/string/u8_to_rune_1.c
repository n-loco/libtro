#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code LATIN_CAPITAL_LETTER_A[] = "A";

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(LATIN_CAPITAL_LETTER_A, &rune);

	bool success = read == 1 && rune == U'A';

	return !success;
}
