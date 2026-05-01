#include <stddef.h>
#include <assert.h>
#include <string.h>

#include <tro/string.h>
#include <tro/dybuffer.h>

int main(void)
{
	tro_strdybuf *buf = tro_create_strdybuf(256);
	assert(buf != NULL);

	tro_sfmt(tro_strdybuf_dybuffer(buf), "Texto %s %d\n", "random", 1);

	size_t strl;
	char *str = tro_strdybuf_getlloc(buf, &strl);
	tro_destroy_strdybuf(buf);

	assert(strl == 15);
	assert(strcmp(str, "Texto random 1\n") == 0);
}
