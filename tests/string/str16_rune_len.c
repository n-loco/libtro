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

#define rs(...)                                                                \
	(const char16_t[])                                                     \
	{                                                                      \
		__VA_ARGS__, 0x0000                                            \
	}

int main(void)
{
	size_t en = tro_str16_urune_len(u"Hello", 0);
	urune_len_assert("Hello", 5, en);

	size_t pt = tro_str16_urune_len(u"Olá", 0);
	urune_len_assert("Olá", 3, pt);

	size_t jp = tro_str16_urune_len(u"こんにちは", 0);
	urune_len_assert("こんにちは", 5, jp);

	size_t emoji = tro_str16_urune_len(u"👋😀", 0);
	urune_len_assert("👋😀", 2, emoji);

	size_t cut_hello_world_emoji =
	    tro_str16_urune_len(rs(0xD83D, 0xDC4B, 0xD83C), 0);
	urune_len_assert("Cut \"👋🌎\"", 2, cut_hello_world_emoji);

	size_t broken_emoji_hello_world =
	    tro_str16_urune_len(rs(0xD83D, 0xDC4B, 0xD83C, 0xFFFF), 0);
	urune_len_assert("Broken \"👋🌎\"", 3, broken_emoji_hello_world);
}
