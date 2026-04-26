#include "tro/string.h"
#include "tro/dybuffer.h"

#include <stddef.h>
#include <stdbool.h>
#include <uchar.h>
#include <stdlib.h>

#ifdef USE_CHAR16_T
# define CHAR_T char16_t

# define tro_strdybuf_T tro_str16dybuf
# define tro_strdybuf_dybuffer_vt_T tro_str16dybuf_dybuffer_vt

# define tro_create_strdybuf_T tro_create_str16dybuf
# define tro_destroy_strdybuf_T tro_destroy_str16dybuf
# define tro_strdybuf_get_T tro_str16dybuf_get
# define tro_strdybuf_getlloc_T tro_str16dybuf_getlloc

# define tro_strdybuf_writes_T tro_str16dybuf_writes
# define tro_strdybuf_writes16_T tro_str16dybuf_writes16
# define tro_strdybuf_writeb_T tro_str16dybuf_writeb
# define tro_strdybuf_writec_T tro_str16dybuf_writec
#else
# define CHAR_T char

# define tro_strdybuf_T tro_strdybuf
# define tro_strdybuf_dybuffer_vt_T tro_strdybuf_dybuffer_vt

# define tro_create_strdybuf_T tro_create_strdybuf
# define tro_destroy_strdybuf_T tro_destroy_strdybuf
# define tro_strdybuf_get_T tro_strdybuf_get
# define tro_strdybuf_getlloc_T tro_strdybuf_getlloc

# define tro_strdybuf_writes_T tro_strdybuf_writes
# define tro_strdybuf_writes16_T tro_strdybuf_writes16
# define tro_strdybuf_writeb_T tro_strdybuf_writeb
# define tro_strdybuf_writec_T tro_strdybuf_writec
#endif

typedef struct mempage mempage;
typedef struct mempage {
	mempage *next;
	size_t datalen;
	CHAR_T data[];
} mempage;

static mempage *alloc_mempage(size_t capacity)
{
	mempage *page = malloc(sizeof(mempage) + (capacity * sizeof(CHAR_T)));
	{
		page->datalen = 0;
		page->next    = NULL;
	}
	return page;
}

struct tro_strdybuf_T {
	size_t pagecap;
	mempage *first;
	mempage *last;
};
static const tro_dybuffer_i dybuffer_impl;
const tro_dybuffer_i *const tro_strdybuf_dybuffer_vt_T = &dybuffer_impl;

tro_strdybuf_T *tro_create_strdybuf_T(size_t pagecap)
{
	if (pagecap == 0)
		return NULL;

	mempage *first = alloc_mempage(pagecap);

	tro_strdybuf_T *buf = malloc(sizeof(tro_strdybuf_T));
	{
		buf->pagecap = pagecap;
		buf->first   = first;
		buf->last    = first;
	}

	return buf;
}

void tro_destroy_strdybuf_T(tro_strdybuf_T *buf)
{
	if (buf == NULL)
		return;

	mempage *page = buf->first;
	for (;;) {
		if (page == NULL)
			break;
		mempage *next = page->next;
		free(page);
		page = next;
	}
	free(buf);
}

size_t tro_strdybuf_get_T(tro_strdybuf_T *buf, CHAR_T *out, size_t outcap)
{
	if (buf == NULL || (out != NULL && outcap == 0))
		return 0;

	size_t strl = 0;

	mempage *page = buf->first;

	if (out == NULL) {
		for (;;) {
			if (page == NULL)
				break;

			strl += page->datalen;
			page  = page->next;
		}
	} else {
		const size_t outcap_bn = outcap - 1;
		for (;;) {
			if (page == NULL)
				break;

			for (size_t i = 0; i < page->datalen; i++) {
				if (strl == outcap_bn)
					goto BREAK_PAGE_FOR;
				out[strl++] = page->data[i];
			}

			page = page->next;

			continue;
		BREAK_PAGE_FOR:
			break;
		}
		out[strl] = '\0';
	}

	return strl;
}

CHAR_T *tro_strdybuf_getlloc_T(tro_strdybuf_T *buf, size_t *outlen)
{
	if (buf == NULL) {
		*outlen = 0;
		return NULL;
	}

	const size_t strl = tro_strdybuf_get_T(buf, NULL, 0);
	CHAR_T *str       = malloc((strl + 1) * sizeof(CHAR_T));
	tro_strdybuf_get_T(buf, str, strl + 1);

	if (outlen != NULL)
		*outlen = strl;
	return str;
}

bool tro_strdybuf_writeb_T(tro_strdybuf_T *buf, const uint8_t *data,
                           size_t datal)
{
	if (buf == NULL)
		return false;

	const size_t pagecap = buf->pagecap;

	mempage *page = buf->last;

	for (size_t i = 0; i < datal; i++) {
		if (page->datalen == pagecap) {
			mempage *new_page = alloc_mempage(pagecap);

			page->next = new_page;
			buf->last  = new_page;

			page = new_page;
		}

		page->data[page->datalen++] = (CHAR_T)data[i];
	}

	return true;
}

static const tro_dybuffer_i dybuffer_impl = {
    .writes = (bool (*)(void *, const char *, size_t))tro_strdybuf_writes_T,
    .writes16 =
        (bool (*)(void *, const char16_t *, size_t))tro_strdybuf_writes16_T,
    .writeb = (bool (*)(void *, const uint8_t *, size_t))tro_strdybuf_writeb_T,
    .writec = (bool (*)(void *, uint32_t))tro_strdybuf_writec_T,
};
