#include <stdbool.h>
#include <stddef.h>

#include <tro/string.h>

// 🌎
#define EARTH_GLOBE_AMERICAS 0x01F30E

#define EARTH_GLOBE_AMERICAS_0 0xF0
#define EARTH_GLOBE_AMERICAS_1 0x9F
#define EARTH_GLOBE_AMERICAS_2 0x8C
#define EARTH_GLOBE_AMERICAS_3 0x8E

int main(void)
{
	tro_u8code ch[TRO_MULTI_U8CODE_MAX];
	size_t ch_l = tro_urune_to_u8codes(EARTH_GLOBE_AMERICAS, ch);

	bool expected_len   = ch_l == 4;
	bool expected_bytes = (ch[0] == EARTH_GLOBE_AMERICAS_0) &&
	                      (ch[1] == EARTH_GLOBE_AMERICAS_1) &&
	                      (ch[2] == EARTH_GLOBE_AMERICAS_2) &&
	                      (ch[3] == EARTH_GLOBE_AMERICAS_3);

	bool success = expected_len && expected_bytes;

	return !success;
}
