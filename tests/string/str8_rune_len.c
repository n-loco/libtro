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
	size_t en = tro_str8_urune_len("Hello", 0);
	urune_len_asset("Hello", 5, en);

	size_t pt = tro_str8_urune_len("Olá", 0);
	urune_len_asset("Olá", 3, pt);

	size_t jp = tro_str8_urune_len("こんにちは", 0);
	urune_len_asset("こんにちは", 5, jp);

	size_t emoji = tro_str8_urune_len("👋😀", 0);
	urune_len_asset("👋😀", 2, emoji);
}
