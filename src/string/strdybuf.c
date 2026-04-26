#include "strdybuf_T.c"

#include <stddef.h>
#include <uchar.h>
#include <string.h>

#include "tro/string.h"

bool tro_strdybuf_writes(tro_strdybuf *buf, const char *data, size_t datal)
{
	if (buf == NULL)
		return false;

	if (datal == 0)
		datal = strlen(data);

	const size_t pagecap = buf->pagecap;

	mempage *page = buf->last;

	for (size_t i = 0; i < datal; i++) {
		if (page->datalen == pagecap) {
			mempage *new_page = alloc_mempage(pagecap);

			page->next = new_page;
			buf->last  = new_page;

			page = new_page;
		}

		page->data[page->datalen++] = data[i];
	}

	return true;
}

bool tro_strdybuf_writes16(tro_strdybuf *buf, const char16_t *data,
                           size_t datal)
{
	if (buf == NULL)
		return false;

	if (datal == 0)
		datal = tro_str16len(data);

	const size_t pagecap = buf->pagecap;

	mempage *page = buf->last;

	size_t i = 0;
	while (i < datal) {
		const tro_u16code *seq = data + i;
		const size_t seqlen    = datal - i;

		tro_u8code bytes[TRO_MULTI_U8CODE_MAX];
		size_t bytesn;
		i += tro_u16codes_to_u8codes(seq, seqlen, bytes, &bytesn);

		for (size_t j = 0; j < bytesn; j++) {
			if (page->datalen == pagecap) {
				mempage *new_page = alloc_mempage(pagecap);

				page->next = new_page;
				buf->last  = new_page;

				page = new_page;
			}

			page->data[page->datalen++] = bytes[j];
		}
	}

	return true;
}

bool tro_strdybuf_writec(tro_strdybuf *buf, uint32_t c32)
{
	if (buf == NULL)
		return false;

	const tro_urune rune = c32;

	tro_u8code bytes[TRO_MULTI_U8CODE_MAX];
	const size_t bytesn = tro_urune_to_u8codes(rune, bytes);

	const size_t pagecap = buf->pagecap;

	mempage *page = buf->last;

	for (size_t i = 0; i < bytesn; i++) {
		if (page->datalen == pagecap) {
			mempage *new_page = alloc_mempage(pagecap);

			page->next = new_page;
			buf->last  = new_page;

			page = new_page;
		}

		page->data[page->datalen++] = bytes[i];
	}

	return true;
}
