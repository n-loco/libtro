#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

// Code point inválido (maior que U+10FFFF)
#define INVALID_CODE 0xFFFFFF

#define REPLACEMENT_CHARACTER_0 0xEF
#define REPLACEMENT_CHARACTER_1 0xBF
#define REPLACEMENT_CHARACTER_2 0xBD

int main(void)
{
	tro_u8code ch[TRO_MULTI_U8CODE_MAX];
	size_t ch_l = tro_urune_to_u8codes(INVALID_CODE, ch);

	bool expected_len   = ch_l == 3;
	bool expected_bytes = (ch[0] == REPLACEMENT_CHARACTER_0) &&
	                      (ch[1] == REPLACEMENT_CHARACTER_1) &&
	                      (ch[2] == REPLACEMENT_CHARACTER_2);

	bool success = expected_len && expected_bytes;

	return !success;
}
