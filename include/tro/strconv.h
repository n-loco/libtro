#ifndef TRO_STRCONV_H_
#define TRO_STRCONV_H_

#include "tro/private/api.h"

#include <stddef.h>
#include <stdint.h>

TRO__C_API_BEGIN

TRO__API size_t tro_int2str(int64_t num, char *out, size_t outcap);

TRO__API size_t tro_uint2str(uint64_t num, char *out, size_t outcap);

TRO__C_API_END

#endif // TRO_STRCONV_H_
