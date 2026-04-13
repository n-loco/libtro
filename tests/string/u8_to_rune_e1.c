#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code INVALID_BYTE_1[] = "\xBF\x8F";
static const tro_u8code INVALID_BYTE_2[] = "\xFF\x8F";

#define REPLACEMENT_CHARACTER 0x00FFFD

int main(void)
{
	tro_urune rune1, rune2;
	size_t read1 = tro_u8codes_to_urune(INVALID_BYTE_1, &rune1);
	size_t read2 = tro_u8codes_to_urune(INVALID_BYTE_2, &rune2);

	bool success1 = read1 == 1 && rune1 == REPLACEMENT_CHARACTER;
	bool success2 = read2 == 1 && rune2 == REPLACEMENT_CHARACTER;

	return !(success1 && success2);
}
