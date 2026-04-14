#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

#define LATIN_CAPITAL_LETTER_A_WITH_ACUTE_0 0xC3
#define LATIN_CAPITAL_LETTER_A_WITH_ACUTE_1 0x81

int main(void)
{
	tro_u8code ch[TRO_MULTI_U8CODE_MAX];
	size_t ch_l = tro_urune_to_u8codes(U'Á', ch);

	bool expected_len   = ch_l == 2;
	bool expected_bytes = (ch[0] == LATIN_CAPITAL_LETTER_A_WITH_ACUTE_0) &&
	                      (ch[1] == LATIN_CAPITAL_LETTER_A_WITH_ACUTE_1);

	bool success = expected_len && expected_bytes;

	return !success;
}
