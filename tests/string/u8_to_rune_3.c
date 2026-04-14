#include <stddef.h>
#include <stdbool.h>

#include <tro/string.h>

static const tro_u8code CJK_IDEOGRAPH[] = "界";

int main(void)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(CJK_IDEOGRAPH, &rune);

	bool success = read == 3 && rune == U'界';

	return !success;
}
