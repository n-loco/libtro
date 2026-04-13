#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

// Á
#define LATIN_CAPITAL_LETTER_A_WITH_ACUTE 0x0000C1

int main(void)
{
	tro_u16code ch[TRO_MULTI_U16CODE_MAX];
	size_t ch_l =
	    tro_urune_to_u16codes(LATIN_CAPITAL_LETTER_A_WITH_ACUTE, ch);

	bool success = ch_l == 1 && ch[0] == LATIN_CAPITAL_LETTER_A_WITH_ACUTE;

	return !success;
}
