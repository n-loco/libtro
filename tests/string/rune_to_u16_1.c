#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

int main(void)
{
	tro_u16code ch[TRO_MULTI_U16CODE_MAX];
	size_t ch_l = tro_urune_to_u16codes(U'Á', ch);

	bool success = ch_l == 1 && ch[0] == u'Á';

	return !success;
}
