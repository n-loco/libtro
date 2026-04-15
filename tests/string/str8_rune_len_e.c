#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <tro/string.h>

#define urune_len_asset(name, expected, got)                                   \
	{                                                                      \
		if (expected != got) {                                         \
			fprintf(stderr, "On \"%s\": expected %zu, got %zu\n",  \
			        name, (size_t)expected, got);                  \
			return 1;                                              \
		}                                                              \
	}

int main(void)
{
	size_t invalid_bytes = tro_str8_urune_len("\xBF\x8F\xFF\x8F", 0);
	urune_len_asset("Bytes inválidos", 4, invalid_bytes);

	size_t cut_sekai = tro_str8_urune_len("世\xE7\x95", 0);
	urune_len_asset("\"世界\" cortado", 2, cut_sekai);

	size_t broken_emoji_hello_world =
	    tro_str8_urune_len("👋\xF0\x9F\xFF\x8E", 0);
	urune_len_asset("\"👋🌎\" quebrado", 4, broken_emoji_hello_world);

	size_t u16_reserved = tro_str8_urune_len("\xED\xA3\xBF-16", 0);
	urune_len_asset("<reservado UTF-16>-16", 4, u16_reserved);
}
