#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

// A
#define LATIN_CAPITAL_LETTER_A 0x000041

int main(void)
{
	tro_u8code ch[TRO_MULTI_U8CODE_MAX];
	size_t ch_l = tro_urune_to_u8codes(LATIN_CAPITAL_LETTER_A, ch);

	bool success = ch_l == 1 && ch[0] == LATIN_CAPITAL_LETTER_A;

	return !success;
}
