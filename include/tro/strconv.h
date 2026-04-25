#ifndef TRO_STRCONV_H_
#define TRO_STRCONV_H_

#include "tro/private/api.h"

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

TRO__C_API_BEGIN

#define TRO_INT_CHAR_MAX 21
#define TRO_UINT_CHAR_MAX 20

TRO__API size_t tro_int2str(int64_t num, char *out, size_t outcap);

TRO__API size_t tro_uint2str(uint64_t num, char *out, size_t outcap);

TRO__API size_t tro_int2str16(int64_t num, char16_t *out, size_t outcap);

TRO__API size_t tro_uint2str16(uint64_t num, char16_t *out, size_t outcap);

TRO__C_API_END

#endif // TRO_STRCONV_H_
