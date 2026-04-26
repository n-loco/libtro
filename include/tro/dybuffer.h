#ifndef TRO_DYBUFFER_H_
#define TRO_DYBUFFER_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>
#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif

TRO__C_API_BEGIN

typedef struct tro_dybuffer_i {
	bool (*const writes)(void *buf, const char *data, size_t datal);
	bool (*const writes16)(void *buf, const char16_t *data, size_t datal);
	bool (*const writeb)(void *buf, const uint8_t *data, size_t datal);
	bool (*const writec)(void *buf, uint32_t c32);
} tro_dybuffer_i;

typedef struct tro_dybuffer_obj {
	void *const obj;
	const tro_dybuffer_i *const vtable;
} tro_dybuffer_obj;

#define tro_dispatch_dybuffer(obj, varname)                                    \
	void *const varname = obj.obj;                                         \
                                                                               \
	bool (*const varname##_writes)(void *buf, const char *data,            \
	                               size_t datal) = obj.vtable->writes;     \
                                                                               \
	bool (*const varname##_writes16)(void *buf, const char16_t *data,      \
	                                 size_t datal) = obj.vtable->writes16; \
                                                                               \
	bool (*const varname##_writeb)(void *buf, const uint8_t *data,         \
	                               size_t datal) = obj.vtable->writeb;     \
                                                                               \
	bool (*const varname##_writec)(void *buf, uint32_t c32) =              \
	    obj.vtable->writec;                                                \
                                                                               \
	/* Supressão de warnings. */ {                                         \
		(void)varname;                                                 \
		(void)varname##_writes;                                        \
		(void)varname##_writes16;                                      \
		(void)varname##_writeb;                                        \
		(void)varname##_writec;                                        \
	}

TRO__C_API_END

#endif // TRO_DYBUFFER_H_
