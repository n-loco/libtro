#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code LATIN_CAPITAL_LETTER_A_WITH_ACUTE[] = "Á";
#define LATIN_CAPITAL_LETTER_A_WITH_ACUTE_U 0x0000C1

int main(void)
{
	tro_urune rune;
	size_t read =
	    tro_u8codes_to_urune(LATIN_CAPITAL_LETTER_A_WITH_ACUTE, &rune);

	bool success = read == 2 && rune == LATIN_CAPITAL_LETTER_A_WITH_ACUTE_U;

	return !success;
}
