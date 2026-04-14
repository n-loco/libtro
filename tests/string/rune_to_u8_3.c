#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

#define CJK_IDEOGRAPH_0 0xE7
#define CJK_IDEOGRAPH_1 0x95
#define CJK_IDEOGRAPH_2 0x8C

int main(void)
{
	tro_u8code ch[TRO_MULTI_U8CODE_MAX];
	size_t ch_l = tro_urune_to_u8codes(U'界', ch);

	bool expected_len   = ch_l == 3;
	bool expected_bytes = (ch[0] == CJK_IDEOGRAPH_0) &&
	                      (ch[1] == CJK_IDEOGRAPH_1) &&
	                      (ch[2] == CJK_IDEOGRAPH_2);

	bool success = expected_len && expected_bytes;

	return !success;
}
