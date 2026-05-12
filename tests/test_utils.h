#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <tro/string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tro/uchar.h"

#define str_assert(name, es, el, gs, gl)                                       \
	{                                                                      \
		if ((size_t)el != (size_t)gl) {                                \
			fprintf(stderr,                                        \
			        "On \"%s\", unmatched lens: expected %zu, "    \
			        "got %zu\n",                                   \
			        name, (size_t)el, (size_t)gl);                 \
			return 1;                                              \
		}                                                              \
		if (strcmp(es, gs) != 0) {                                     \
			fprintf(stderr,                                        \
			        "On \"%s\", unmatched string: expected "       \
			        "\"%s\", got \"%s\"\n",                        \
			        name, es, gs);                                 \
			return 1;                                              \
		}                                                              \
	}

#define str16_assert(name, es, el, gs, gl)                                     \
	if (!impl__str16_assert(name, es, el, gs, gl))                         \
		return 1;

#define len_assert(name, el, gl)                                               \
	{                                                                      \
		if ((size_t)el != (size_t)gl) {                                \
			fprintf(stderr,                                        \
			        "On \"%s\", unmatched lens: expected %zu, "    \
			        "got %zu\n",                                   \
			        name, (size_t)el, (size_t)gl);                 \
			return 1;                                              \
		}                                                              \
	}

static inline bool impl__str16_assert(const char *name, const tro_char16 *es,
                                      size_t el, const tro_char16 *gs,
                                      size_t gl)
{
	if (el != gl) {
		fprintf(stderr,
		        "On \"%s\", unmatched lens: expected %zu, "
		        "got %zu\n",
		        name, el, gl);
		return false;
	}

	if (tro_str16cmp(es, gs) != 0) {
		char *u8es = tro_cnvlloc_str16_to_str(es, el, NULL);
		char *u8gs = tro_cnvlloc_str16_to_str(gs, gl, NULL);

		fprintf(stderr,
		        "On \"%s\", unmatched string: expected "
		        "\"%s\", got \"%s\"\n",
		        name, u8es, u8gs);

		free(u8gs);
		free(u8es);
		return false;
	}

	return true;
}

#endif // TEST_UTILS_H
