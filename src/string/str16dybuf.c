#define USE_CHAR16_T
#include "strdybuf_T.c"

#include <stddef.h>
#include <uchar.h>
#include <string.h>

#include "tro/string.h"

bool tro_str16dybuf_writes(tro_str16dybuf *buf, const char *data, size_t datal)
{
	if (buf == NULL)
		return false;

	if (datal == 0)
		datal = strlen(data);

	const size_t pagecap = buf->pagecap;

	mempage *page = buf->last;

	size_t i = 0;
	while (i < datal) {
		const tro_u8code *seq = (tro_u8code *)(data + i);
		const size_t seqlen   = datal - i;

		tro_u16code surrs[TRO_MULTI_U16CODE_MAX];
		size_t surrsn;
		i += tro_u8codes_to_u16codes(seq, seqlen, surrs, &surrsn);

		for (size_t j = 0; j < surrsn; j++) {
			if (page->datalen == pagecap) {
				mempage *new_page = alloc_mempage(pagecap);

				page->next = new_page;
				buf->last  = new_page;

				page = new_page;
			}

			page->data[page->datalen++] = surrs[j];
		}
	}

	return true;
}

bool tro_str16dybuf_writes16(tro_str16dybuf *buf, const char16_t *data,
                             size_t datal)
{
	if (buf == NULL)
		return false;

	if (datal == 0)
		datal = tro_str16len(data);

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

bool tro_str16dybuf_writec(tro_str16dybuf *buf, uint32_t c32)
{
	if (buf == NULL)
		return false;

	const tro_urune rune = c32;

	tro_u16code surrs[TRO_MULTI_U16CODE_MAX];
	const size_t surrsn = tro_urune_to_u16codes(rune, surrs);

	const size_t pagecap = buf->pagecap;

	mempage *page = buf->last;

	for (size_t i = 0; i < surrsn; i++) {
		if (page->datalen == pagecap) {
			mempage *new_page = alloc_mempage(pagecap);

			page->next = new_page;
			buf->last  = new_page;

			page = new_page;
		}

		page->data[page->datalen++] = surrs[i];
	}

	return true;
}
