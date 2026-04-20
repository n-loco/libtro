#include "tro/string.h"

#include <stddef.h>
#include <uchar.h>
#include <stdbool.h>

size_t tro_str16len(const char16_t *str)
{
	size_t len = 0;
	while (*str) {
		len++;
		str++;
	}
	return len;
}

size_t tro_str16nlen(const char16_t *str, size_t maxlen)
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

int tro_str16cmp(const char16_t *s1, const char16_t *s2)
{
	int diff = 0;

	while (true) {
		const char16_t c1 = *s1;
		const char16_t c2 = *s2;

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

int tro_str16ncmp(const char16_t *s1, const char16_t *s2, size_t maxlen)
{
	int diff = 0;

	for (size_t i = 0; i < maxlen; i++) {
		const char16_t c1 = *s1;
		const char16_t c2 = *s2;

		const bool is_diff = diff != 0;
		const bool is_eos  = c1 == '\0' && c2 == '\0';

		if (is_diff || is_eos)
			break;

		s1++;
		s2++;
	}

	return diff;
}
