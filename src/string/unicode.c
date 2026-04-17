#include "tro/string.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

size_t tro_conv_str_to_str16(const char *in, size_t in_len, char16_t *out,
                             size_t out_cap)
{
	if (out == NULL || out_cap == 0)
		return 0;
	if (in == NULL) {
		if (out_cap > 1)
			*out = '\0';
		return 0;
	}

	if (in_len == 0)
		in_len = strlen(in);

	const size_t out_cap_bn = out_cap - 1;
	size_t written_out      = 0;

	size_t i = 0;
	while (i < in_len) {
		const tro_u8code *seq = (const tro_u8code *)(in + i);
		const size_t seq_len = in_len - 1;

		tro_urune rune;
		i += tro_u8codes_to_urune(seq, seq_len, &rune);

		tro_u16code surrogates[TRO_MULTI_U16CODE_MAX];
		size_t surrogates_n = tro_urune_to_u16codes(rune, surrogates);

		const size_t remaining_capacity = out_cap_bn - written_out;
		if (remaining_capacity < surrogates_n) {
			out[written_out] = u'�';
			written_out++;
			break;
		}

		out[written_out] = surrogates[0];
		written_out++;

		if (surrogates_n > 1) {
			out[written_out] = surrogates[1];
			written_out++;
		}

		if (written_out == out_cap_bn)
			break;
	}

	out[written_out] = '\0';
	return written_out;
}

char16_t *tro_cnvlloc_str_to_str16(const char *in, size_t in_len,
                                   size_t *len_out)
{
	if (in == NULL) {
		*len_out = 0;
		return NULL;
	}

	if (in_len == 0)
		in_len = strlen(in);

	const size_t str_mem =
	    (tro_str8_urune_len(in, in_len) * sizeof(tro_urune)) + 2;
	const size_t str_cap = str_mem / 2;

	char16_t *str  = malloc(str_mem);
	size_t str_len = tro_conv_str_to_str16(in, in_len, str, str_cap);

	if (len_out != NULL)
		*len_out = str_len;
	return str;
}
