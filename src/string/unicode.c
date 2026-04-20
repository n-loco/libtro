#include "tro/string.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

size_t tro_conv_str_to_str16(const char *in, size_t inlen, char16_t *out,
                             size_t outcap)
{
	if (out != NULL && outcap == 0)
		return 0;

	if (in == NULL) {
		if (out != NULL && outcap > 1)
			*out = '\0';
		return 0;
	}

	if (inlen == 0)
		inlen = strlen(in);

	const size_t outcap_bn = out != NULL ? outcap - 1 : SIZE_MAX;
	size_t written_out     = 0;

	size_t i = 0;
	while (i < inlen) {
		const tro_u8code *seq = (const tro_u8code *)(in + i);
		const size_t seqlen   = inlen - 1;

		tro_u16code surrogates[TRO_MULTI_U16CODE_MAX];
		size_t surrogates_n;
		i += tro_u8codes_to_u16codes(seq, seqlen, surrogates,
		                             &surrogates_n);

		if (out != NULL) {
			const size_t remaining_cap = outcap_bn - written_out;

			if (remaining_cap < surrogates_n) {
				out[written_out] = '\0';
				return written_out;
			}

			out[written_out] = surrogates[0];
			written_out++;

			if (surrogates_n > 1) {
				out[written_out] = surrogates[1];
				written_out++;
			}

			if (written_out == outcap_bn)
				break;
		} else {
			written_out += surrogates_n;
		}
	}

	if (out != NULL)
		out[written_out] = '\0';

	return written_out;
}

char16_t *tro_cnvlloc_str_to_str16(const char *in, size_t inlen, size_t *lenout)
{
	if (in == NULL) {
		*lenout = 0;
		return NULL;
	}

	if (inlen == 0)
		inlen = strlen(in);

	const size_t strmem =
	    (tro_str8_urune_len(in, inlen) * sizeof(tro_urune)) + 2;
	const size_t strcap = strmem / 2;

	char16_t *str = malloc(strmem);
	size_t strl   = tro_conv_str_to_str16(in, inlen, str, strcap);

	if (lenout != NULL)
		*lenout = strl;
	return str;
}

size_t tro_conv_str16_to_str(const char16_t *in, size_t inlen, char *out,
                             size_t outcap)
{
	if (out != NULL && outcap == 0)
		return 0;

	if (in == NULL) {
		if (out != NULL && outcap > 1)
			*out = '\0';
		return 0;
	}

	if (inlen == 0)
		inlen = tro_str16len(in);

	const size_t outcap_bn = outcap - 1;
	size_t written_out     = 0;

	size_t i = 0;
	while (i < inlen) {
		const tro_u16code *seq = (const tro_u16code *)(in + i);
		const size_t seqlen    = inlen - 1;

		tro_u8code bytes[TRO_MULTI_U8CODE_MAX + 1] = {0};
		size_t bytes_n;
		i += tro_u16codes_to_u8codes(seq, seqlen, bytes, &bytes_n);

		if (out != NULL) {
			const size_t remaining_cap = outcap_bn - written_out;
			if (remaining_cap < bytes_n) {
				out[written_out] = '\0';
				return written_out;
			}

			for (size_t j = 0; j < bytes_n; j++) {
				out[written_out] = bytes[j];
				written_out++;

				if (written_out == outcap_bn)
					break;
			}
		} else {
			written_out += bytes_n;
		}
	}

	if (out != NULL)
		out[written_out] = '\0';

	return written_out;
}

char *tro_cnvlloc_str16_to_str(const char16_t *in, size_t inlen, size_t *lenout)
{
	if (in == NULL) {
		*lenout = 0;
		return NULL;
	}

	if (inlen == 0)
		inlen = tro_str16len(in);

	const size_t strmem =
	    (tro_str16_urune_len(in, inlen) * sizeof(tro_urune)) + 1;
	const size_t strcap = strmem;

	char *str   = malloc(strmem);
	size_t strl = tro_conv_str16_to_str(in, inlen, str, strcap);

	if (lenout != NULL)
		*lenout = strl;
	return str;
}
