#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

int main(void)
{
	tro_u8code ch[TRO_MULTI_U8CODE_MAX];
	size_t ch_l = tro_urune_to_u8codes(U'A', ch);

	bool success = ch_l == 1 && ch[0] == 'A';

	return !success;
}
