#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include <tro/string.h>

#define urune_len_assert(name, expected, got)                                  \
	{                                                                      \
		if (expected != got) {                                         \
			fprintf(stderr, "On \"%s\": expected %zu, got %zu\n",  \
			        name, (size_t)expected, got);                  \
			return 1;                                              \
		}                                                              \
	}

int main(void)
{
	size_t en = tro_str8_urune_len("Hello", 0);
	urune_len_assert("Hello", 5, en);

	size_t pt = tro_str8_urune_len("Olá", 0);
	urune_len_assert("Olá", 3, pt);

	size_t jp = tro_str8_urune_len("こんにちは", 0);
	urune_len_assert("こんにちは", 5, jp);

	size_t emoji = tro_str8_urune_len("👋😀", 0);
	urune_len_assert("👋😀", 2, emoji);

	size_t invalid_bytes = tro_str8_urune_len("\xBF\x8F\xFF\x8F", 0);
	urune_len_assert("Invalid bytes", 4, invalid_bytes);

	size_t cut_sekai = tro_str8_urune_len("世\xE7\x95", 0);
	urune_len_assert("Cut \"世界\"", 2, cut_sekai);

	size_t broken_emoji_hello_world =
	    tro_str8_urune_len("👋\xF0\x9F\xFF\x8E", 0);
	urune_len_assert("Broken \"👋🌎\"", 4, broken_emoji_hello_world);

	size_t u16_reserved = tro_str8_urune_len("\xED\xA3\xBF-16", 0);
	urune_len_assert("<UTF-16 reserved>-16", 4, u16_reserved);
}
