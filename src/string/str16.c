#include "tro/string.h"

#include <stddef.h>
#include <stdbool.h>

#include "tro/uchar.h"

size_t tro_str16len(const tro_char16 *str)
{
	size_t len = 0;
	while (*str) {
		len++;
		str++;
	}
	return len;
}

size_t tro_str16nlen(const tro_char16 *str, size_t maxlen)
{
	size_t len = 0;
	while (len < maxlen) {
		if (!(*str))
			break;

		len++;
		str++;
	}
	return len;
}

int tro_str16cmp(const tro_char16 *s1, const tro_char16 *s2)
{
	int diff = 0;

	while (true) {
		const tro_char16 c1 = *s1;
		const tro_char16 c2 = *s2;

		diff = c1 - c2;

		const bool is_diff = diff != 0;
		const bool is_eos  = c1 == '\0' && c2 == '\0';

		if (is_diff || is_eos)
			break;

		s1++;
		s2++;
	}

	return diff;
}

int tro_str16ncmp(const tro_char16 *s1, const tro_char16 *s2, size_t maxlen)
{
	int diff = 0;

	for (size_t i = 0; i < maxlen; i++) {
		const tro_char16 c1 = *s1;
		const tro_char16 c2 = *s2;

		const bool is_diff = diff != 0;
		const bool is_eos  = c1 == '\0' && c2 == '\0';

		if (is_diff || is_eos)
			break;

		s1++;
		s2++;
	}

	return diff;
}
