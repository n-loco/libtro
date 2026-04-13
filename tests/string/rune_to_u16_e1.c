#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

// Code point inválido (reservado do UTF-16)
#define INVALID_CODE 0x00D8F3

#define REPLACEMENT_CHARACTER 0xFFFD

int main(void)
{
	tro_u16code ch[TRO_MULTI_U16CODE_MAX];
	size_t ch_l = tro_urune_to_u16codes(INVALID_CODE, ch);

	bool success = ch_l == 1 && ch[0] == REPLACEMENT_CHARACTER;

	return !success;
}
