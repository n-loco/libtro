#ifndef TRO_STRCONV_H_
#define TRO_STRCONV_H_

#include "tro/private/api.h"
#include "tro/private/bool.h"

#include <stddef.h>
#include <stdint.h>
#include <uchar.h>

#if TRO__USE_STDBOOL
# include <stdbool.h>
#endif

TRO__C_API_BEGIN

#define TRO_INT_CHAR_MAX 21
#define TRO_UINT_CHAR_MAX 20

TRO__API size_t tro_int2str(int64_t num, char *out, size_t outcap);

TRO__API size_t tro_uint2str(uint64_t num, char *out, size_t outcap);

TRO__API size_t tro_int2str16(int64_t num, char16_t *out, size_t outcap);

TRO__API size_t tro_uint2str16(uint64_t num, char16_t *out, size_t outcap);

#define TRO_FLOAT_CHAR_MAX_PRECISION 17
#define TRO_FLOAT_FIXED_CHAR_MAX (2 + 309 + TRO_FLOAT_CHAR_MAX_PRECISION)
#define TRO_FLOAT_E_CHAR_MAX 25

static inline size_t tro_float2str(double num, char *out, size_t outcap)
{
	TRO__API size_t tro__float2str_general(double num, uint32_t precision,
	                                       char *out, size_t outcap,
	                                       bool shortest);

	return tro__float2str_general(num, 6, out, outcap, true);
}

static inline size_t tro_float2str_adjustable(double num, uint32_t precision,
                                              char *out, size_t outcap)
{
	TRO__API size_t tro__float2str_general(double num, uint32_t precision,
	                                       char *out, size_t outcap,
	                                       bool shortest);

	return tro__float2str_general(num, precision, out, outcap, true);
}

static inline size_t tro_float2str_fixed(double num, uint32_t precision,
                                         char *out, size_t outcap)
{
	TRO__API size_t tro__float2str_general(double num, uint32_t precision,
	                                       char *out, size_t outcap,
	                                       bool shortest);

	return tro__float2str_general(num, precision, out, outcap, false);
}

static inline size_t tro_float2str_E(double num, char *out, size_t outcap)
{
	TRO__API size_t tro__float2str_eE(double num, char *out, size_t outcap,
	                                  bool capital_e);

	return tro__float2str_eE(num, out, outcap, true);
}

static inline size_t tro_float2str_e(double num, char *out, size_t outcap)
{
	TRO__API size_t tro__float2str_eE(double num, char *out, size_t outcap,
	                                  bool capital_e);

	return tro__float2str_eE(num, out, outcap, false);
}

static inline size_t tro_float2str_fixed_E(double num, uint32_t precision,
                                           char *out, size_t outcap)
{
	TRO__API size_t tro__float2str_fixed_eE(double num, uint32_t precision,
	                                        char *out, size_t outcap,
	                                        bool capital_e);

	return tro__float2str_fixed_eE(num, precision, out, outcap, true);
}

static inline size_t tro_float2str16(double num, char16_t *out, size_t outcap)
{
	TRO__API size_t tro__float2str16_general(double num, uint32_t precision,
	                                         char16_t *out, size_t outcap,
	                                         bool shortest);

	return tro__float2str16_general(num, 6, out, outcap, true);
}

static inline size_t tro_float2str16_adjustable(double num, uint32_t precision,
                                                char16_t *out, size_t outcap)
{
	TRO__API size_t tro__float2str16_general(double num, uint32_t precision,
	                                         char16_t *out, size_t outcap,
	                                         bool shortest);

	return tro__float2str16_general(num, precision, out, outcap, true);
}

static inline size_t tro_float2str16_fixed(double num, uint32_t precision,
                                           char16_t *out, size_t outcap)
{
	TRO__API size_t tro__float2str16_general(double num, uint32_t precision,
	                                         char16_t *out, size_t outcap,
	                                         bool shortest);

	return tro__float2str16_general(num, precision, out, outcap, false);
}

static inline size_t tro_float2str16_E(double num, char16_t *out, size_t outcap)
{
	TRO__API size_t tro__float2str16_eE(double num, char16_t *out,
	                                    size_t outcap, bool capital_e);

	return tro__float2str16_eE(num, out, outcap, true);
}

static inline size_t tro_float2str_fixed_e(double num, uint32_t precision,
                                           char *out, size_t outcap)
{
	TRO__API size_t tro__float2str_fixed_eE(double num, uint32_t precision,
	                                        char *out, size_t outcap,
	                                        bool capital_e);

	return tro__float2str_fixed_eE(num, precision, out, outcap, false);
}

static inline size_t tro_float2str16_e(double num, char16_t *out, size_t outcap)
{
	TRO__API size_t tro__float2str16_eE(double num, char16_t *out,
	                                    size_t outcap, bool capital_e);

	return tro__float2str16_eE(num, out, outcap, false);
}

static inline size_t tro_float2str16_fixed_E(double num, uint32_t precision,
                                             char16_t *out, size_t outcap)
{
	TRO__API size_t tro__float2str16_fixed_eE(
	    double num, uint32_t precision, char16_t *out, size_t outcap,
	    bool capital_e);

	return tro__float2str16_fixed_eE(num, precision, out, outcap, true);
}

static inline size_t tro_float2str16_fixed_e(double num, uint32_t precision,
                                             char16_t *out, size_t outcap)
{
	TRO__API size_t tro__float2str16_fixed_eE(
	    double num, uint32_t precision, char16_t *out, size_t outcap,
	    bool capital_e);

	return tro__float2str16_fixed_eE(num, precision, out, outcap, false);
}

TRO__C_API_END

#endif // TRO_STRCONV_H_
