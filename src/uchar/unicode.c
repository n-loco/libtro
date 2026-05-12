#include "tro/uchar.h"

#include <stddef.h>

size_t tro_u8codes_to_u16codes(const tro_u8code *seq, size_t seqlen,
                               tro_u16code *out, size_t *outlen)
{
	tro_urune rune;
	size_t read = tro_u8codes_to_urune(seq, seqlen, &rune);
	*outlen     = tro_urune_to_u16codes(rune, out);
	return read;
}

size_t tro_u16codes_to_u8codes(const tro_u16code *seq, size_t seqlen,
                               tro_u8code *out, size_t *outlen)
{
	tro_urune rune;
	size_t read = tro_u16codes_to_urune(seq, seqlen, &rune);
	*outlen     = tro_urune_to_u8codes(rune, out);
	return read;
}
