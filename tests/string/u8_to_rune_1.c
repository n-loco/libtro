#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code LATIN_CAPITAL_LETTER_A[] = "A";
#define LATIN_CAPITAL_LETTER_A_U 0x000041

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(LATIN_CAPITAL_LETTER_A, &rune);

	bool success = read == 1 && rune == LATIN_CAPITAL_LETTER_A_U;

	return !success;
}
