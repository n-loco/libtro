#include "tro/string.h"

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#include "tro/dybuffer.h"
#include "tro/strconv.h"
#include "tro/uchar.h"
#include "fmt_parser.h"
#include "utils/math.h"

#ifdef USE_CHAR16_T
# define CHAR_T tro_char16

# define tro_sfmt_T tro_sfmt16
# define tro_vsfmt_T tro_vsfmt16

# define buf_writes_T buf_writes16

# define tro__parse_fmt_spec_T tro__parse_fmt16_spec
# define tro_int2str_T tro_int2str16
# define tro_uint2str_T tro_uint2str16
# define tro_float2str_adjustable_T tro_float2str16_adjustable
# define tro_float2str_fixed_T tro_float2str16_fixed
#else
# define CHAR_T char

# define tro_sfmt_T tro_sfmt
# define tro_vsfmt_T tro_vsfmt

# define buf_writes_T buf_writes

# define tro__parse_fmt_spec_T tro__parse_fmt_spec
# define tro_int2str_T tro_int2str
# define tro_uint2str_T tro_uint2str
# define tro_float2str_adjustable_T tro_float2str_adjustable
# define tro_float2str_fixed_T tro_float2str_fixed
#endif

typedef struct {
	const CHAR_T *text;
	bool was_text;
	bool is_text;
	size_t text_start;
	size_t index;
} write_text_state;

static bool write_text(tro_dybuffer_obj buf, write_text_state *state);

static bool write_pad(tro_dybuffer_obj buf, size_t n, bool zero);

typedef struct {
	const char *str;
	size_t width;
	bool left_justified;
} opts_s;

#define arg_s(ap, fmt, o)                                                      \
	do {                                                                   \
		(o)->str   = va_arg(ap, const char *);                         \
		(o)->width = (fmt)->field_width.from_va                        \
		                 ? (size_t)va_arg(ap, unsigned int)            \
		                 : (fmt)->field_width.width;                   \
                                                                               \
		(o)->left_justified = (fmt)->flags.left_justified;             \
	} while (0)

static bool fmt_s(tro_dybuffer_obj buf, const opts_s *opts);

typedef struct {
	const tro_char16 *str;
	size_t width;
	bool left_justified;
} opts_S;

#define arg_S(ap, fmt, o)                                                      \
	do {                                                                   \
		(o)->str   = va_arg(ap, const tro_char16 *);                   \
		(o)->width = (fmt)->field_width.from_va                        \
		                 ? (size_t)va_arg(ap, unsigned int)            \
		                 : (fmt)->field_width.width;                   \
                                                                               \
		(o)->left_justified = (fmt)->flags.left_justified;             \
	} while (0)

static bool fmt_S(tro_dybuffer_obj buf, const opts_S *opts);

typedef struct {
	tro_urune rune;
	size_t width;
	bool left_justified: 1;
	bool alternative   : 1;
} opts_c;

#define arg_c(ap, fmt, o)                                                      \
	do {                                                                   \
		(o)->rune  = va_arg(ap, tro_urune);                            \
		(o)->width = (fmt)->field_width.from_va                        \
		                 ? (size_t)va_arg(ap, unsigned int)            \
		                 : (fmt)->field_width.width;                   \
                                                                               \
		(o)->left_justified = (fmt)->flags.left_justified;             \
		(o)->alternative    = (fmt)->flags.alternative;                \
	} while (0)

static bool fmt_c(tro_dybuffer_obj buf, const opts_c *opts);

typedef struct {
	int64_t sint;
	size_t width;
	bool left_justified: 1;
	bool zero_pad      : 1;
	bool sign_always   : 1;
} opts_d;

#define arg_d(ap, fmt, o)                                                      \
	do {                                                                   \
		switch ((fmt)->len) {                                          \
		default:                                                       \
			(o)->sint = (int64_t)va_arg(ap, int);                  \
			break;                                                 \
		case FMT_SPEC_LEN_HH:                                          \
			(o)->sint =                                            \
			    (int64_t)(signed char)(va_arg(ap, unsigned int) &  \
			                           UCHAR_MAX);                 \
			break;                                                 \
		case FMT_SPEC_LEN_H:                                           \
			(o)->sint =                                            \
			    (int64_t)(short)(va_arg(ap, unsigned int) &        \
			                     USHRT_MAX);                       \
			break;                                                 \
		case FMT_SPEC_LEN_L:                                           \
			(o)->sint = (int64_t)va_arg(ap, long);                 \
			break;                                                 \
		case FMT_SPEC_LEN_LL:                                          \
			(o)->sint = (int64_t)va_arg(ap, long long);            \
			break;                                                 \
		case FMT_SPEC_LEN_J:                                           \
			(o)->sint = (int64_t)va_arg(ap, intmax_t);             \
			break;                                                 \
		case FMT_SPEC_LEN_Z:                                           \
			/* fall through */                                     \
		case FMT_SPEC_LEN_T:                                           \
			(o)->sint = (int64_t)va_arg(ap, ptrdiff_t);            \
			break;                                                 \
		}                                                              \
                                                                               \
		(o)->width = (fmt)->field_width.from_va                        \
		                 ? (size_t)va_arg(ap, unsigned int)            \
		                 : (fmt)->field_width.width;                   \
                                                                               \
		(o)->left_justified = (fmt)->flags.left_justified;             \
		(o)->zero_pad       = (fmt)->flags.zero_pad;                   \
		(o)->sign_always    = (fmt)->flags.sign_always;                \
	} while (0)

static bool fmt_d(tro_dybuffer_obj buf, const opts_d *opts);

typedef struct {
	uint64_t uint;
	size_t width;
	bool left_justified: 1;
	bool zero_pad      : 1;
	bool sign_always   : 1;
} opts_oxXu;

#define arg_oxXu(ap, fmt, o)                                                   \
	do {                                                                   \
		switch ((fmt)->len) {                                          \
		default:                                                       \
			(o)->uint = (uint64_t)va_arg(ap, unsigned int);        \
			break;                                                 \
		case FMT_SPEC_LEN_HH:                                          \
			(o)->uint =                                            \
			    (uint64_t)(unsigned char)(va_arg(ap,               \
			                                     unsigned int) &   \
			                              UCHAR_MAX);              \
			break;                                                 \
		case FMT_SPEC_LEN_H:                                           \
			(o)->uint =                                            \
			    (uint64_t)(unsigned short)(va_arg(ap,              \
			                                      unsigned int) &  \
			                               USHRT_MAX);             \
			break;                                                 \
		case FMT_SPEC_LEN_L:                                           \
			(o)->uint = (uint64_t)va_arg(ap, unsigned long);       \
			break;                                                 \
		case FMT_SPEC_LEN_LL:                                          \
			(o)->uint = (uint64_t)va_arg(ap, unsigned long long);  \
			break;                                                 \
		case FMT_SPEC_LEN_J:                                           \
			(o)->uint = (uint64_t)va_arg(ap, uintmax_t);           \
			break;                                                 \
		case FMT_SPEC_LEN_Z:                                           \
			/* fall through */                                     \
		case FMT_SPEC_LEN_T:                                           \
			(o)->uint = (uint64_t)va_arg(ap, size_t);              \
			break;                                                 \
		}                                                              \
                                                                               \
		(o)->width = (fmt)->field_width.from_va                        \
		                 ? (size_t)va_arg(ap, unsigned int)            \
		                 : (fmt)->field_width.width;                   \
                                                                               \
		(o)->left_justified = (fmt)->flags.left_justified;             \
		(o)->zero_pad       = (fmt)->flags.zero_pad;                   \
		(o)->sign_always    = (fmt)->flags.sign_always;                \
	} while (0)

static bool fmt_u(tro_dybuffer_obj buf, const opts_oxXu *opts);

typedef struct {
	double_t dfloat;
	size_t width;
	size_t precision;
	bool left_justified: 1;
	bool zero_pad      : 1;
	bool sign_always   : 1;
	bool alternative   : 1;
} opts_feEgGaA;

#define arg_feEgGaA(ap, fmt, o)                                                \
	do {                                                                   \
		if ((fmt)->len == FMT_SPEC_LEN_CL)                             \
			(o)->dfloat = (double)va_arg(ap, long double);         \
		else                                                           \
			(o)->dfloat = va_arg(ap, double);                      \
                                                                               \
		(o)->width = (fmt)->field_width.from_va                        \
		                 ? (size_t)va_arg(ap, unsigned int)            \
		                 : (fmt)->field_width.width;                   \
                                                                               \
		(o)->precision = (fmt)->precision_width.dot                    \
		                     ? (fmt)->precision_width.from_va          \
		                           ? (size_t)va_arg(ap, unsigned int)  \
		                           : (fmt)->precision_width.width      \
		                     : 6;                                      \
                                                                               \
		(o)->left_justified = (fmt)->flags.left_justified;             \
		(o)->zero_pad       = (fmt)->flags.zero_pad;                   \
		(o)->sign_always    = (fmt)->flags.sign_always;                \
		(o)->alternative    = (fmt)->flags.alternative;                \
	} while (0)

static bool fmt_f(tro_dybuffer_obj buf, const opts_feEgGaA *opts);

typedef struct {
	uint64_t ptr;
	size_t width;
	bool left_justified;
} opts_p;

#define arg_p(ap, fmt, o)                                                      \
	do {                                                                   \
		(o)->ptr = (uint64_t)va_arg(ap, uintptr_t);                    \
                                                                               \
		(o)->width = (fmt)->field_width.from_va                        \
		                 ? (size_t)va_arg(ap, unsigned int)            \
		                 : (fmt)->field_width.width;                   \
                                                                               \
		(o)->left_justified = (fmt)->flags.left_justified;             \
	} while (0)

static inline bool (*buf_get_writes_DYN(tro_dybuffer_obj buf_obj))(void *,
                                                                   const void *,
                                                                   size_t);

bool tro_sfmt_T(tro_dybuffer_obj buf, const CHAR_T *format, ...)
{
	va_list args;
	va_start(args, format);
	bool r = tro_vsfmt_T(buf, format, args);
	va_end(args);
	return r;
}

bool tro_vsfmt_T(tro_dybuffer_obj buf_obj, const CHAR_T *format, va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	write_text_state text_state = {.text = format};

	for (size_t i = 0;;) {
		fmt_specifier spec;
		const size_t specl = tro__parse_fmt_spec_T(format + i, &spec);
		text_state.is_text = spec.type == FMT_SPEC_NONE;
		text_state.index   = i;

		if (!write_text(buf_obj, &text_state))
			return false;

		switch (spec.type) {
		case FMT_SPEC_NONE:
			break; // pass.
		case FMT_SPEC_EOS:
			return true;
		case FMT_SPEC_ERROR:
			if (!buf_writec(buf, u'␀', 1))
				return false;
			break;
		case FMT_SPEC_ESC:
			if (!buf_writec(buf, '%', 1))
				return false;
			break;
		case FMT_SPEC_C: {
			opts_c opts;
			arg_c(args, &spec, &opts);
			if (!fmt_c(buf_obj, &opts))
				return false;
		} break;
		case FMT_SPEC_S: {
			opts_s opts;
			arg_s(args, &spec, &opts);
			if (!fmt_s(buf_obj, &opts))
				return false;
		} break;
		case FMT_SPEC_CS: {
			opts_S opts;
			arg_S(args, &spec, &opts);
			if (!fmt_S(buf_obj, &opts))
				return false;
		} break;
		case FMT_SPEC_D: {
			opts_d opts;
			arg_d(args, &spec, &opts);
			if (!fmt_d(buf_obj, &opts))
				return false;
		} break;
		case FMT_SPEC_O: {
			// TODO!
			opts_oxXu opts;
			arg_oxXu(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_X: {
			// TODO!
			opts_oxXu opts;
			arg_oxXu(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_CX: {
			// TODO!
			opts_oxXu opts;
			arg_oxXu(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_U: {
			opts_oxXu opts;
			arg_oxXu(args, &spec, &opts);
			if (!fmt_u(buf_obj, &opts))
				return false;
		} break;
		case FMT_SPEC_F: {
			opts_feEgGaA opts;
			arg_feEgGaA(args, &spec, &opts);
			if (!fmt_f(buf_obj, &opts))
				return false;
		} break;
		case FMT_SPEC_E: {
			// TODO!
			opts_feEgGaA opts;
			arg_feEgGaA(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_CE: {
			// TODO!
			opts_feEgGaA opts;
			arg_feEgGaA(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_G: {
			// TODO!
			opts_feEgGaA opts;
			arg_feEgGaA(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_CG: {
			// TODO!
			opts_feEgGaA opts;
			arg_feEgGaA(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_A: {
			// TODO!
			opts_feEgGaA opts;
			arg_feEgGaA(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_CA: {
			// TODO!
			opts_feEgGaA opts;
			arg_feEgGaA(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		case FMT_SPEC_P: {
			// TODO!
			opts_p opts;
			arg_p(args, &spec, &opts);
			if (!buf_writec(buf, u'␀', 1))
				return false;
		} break;
		}

		i += specl;
	}
}

static bool write_text(tro_dybuffer_obj buf_obj, write_text_state *state)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	if (state->is_text) {
		if (!state->was_text)
			state->text_start = state->index;
	} else {
		if (state->was_text) {
			const CHAR_T *str = state->text + state->text_start;
			const size_t strl = state->index - state->text_start;
			if (!buf_writes_T(buf, str, strl))
				return false;
		}
	}
	state->was_text = state->is_text;

	return true;
}

static bool write_pad(tro_dybuffer_obj buf_obj, size_t n, bool zero)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	const tro_urune c = zero ? '0' : ' ';
	return buf_writec(buf, c, n);
}

static bool fmt_s(tro_dybuffer_obj buf_obj, const opts_s *opts)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	const char *const str = opts->str;
	const size_t strl     = strlen(str);
	const size_t width    = opts->width;
	const bool left_just  = opts->left_justified;

	if (width > 0) {
		const size_t strw  = tro_str8_urune_len(str, strl);
		const size_t justw = MAX(width, strw) - strw;

		if (left_just) {
			if (!buf_writes(buf, str, strl))
				return false;
			return write_pad(buf_obj, justw, false);
		} else {
			if (!write_pad(buf_obj, justw, false))
				return false;
			return buf_writes(buf, str, strl);
		}
	}

	return buf_writes(buf, str, strl);
}

static bool fmt_S(tro_dybuffer_obj buf_obj, const opts_S *opts)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	const tro_char16 *const str = opts->str;
	const size_t strl           = tro_str16len(str);
	const size_t width          = opts->width;
	const bool left_just        = opts->left_justified;

	if (width > 0) {
		const size_t strw  = tro_str16_urune_len(str, strl);
		const size_t justw = MAX(width, strw) - strw;

		if (left_just) {
			if (!buf_writes16(buf, str, strl))
				return false;
			return write_pad(buf_obj, justw, false);
		} else {
			if (!write_pad(buf_obj, justw, false))
				return false;
			return buf_writes16(buf, str, strl);
		}
	}

	return buf_writes16(buf, str, strl);
}

static bool fmt_c(tro_dybuffer_obj buf_obj, const opts_c *opts)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	const tro_urune rune   = opts->rune;
	const bool alternative = opts->alternative;
	const bool left_just   = opts->left_justified;

	size_t width = opts->width;

	if (alternative) {
		if (width == 0)
			width = 1;

		return buf_writec(buf, rune, width);
	}

	if (width >= 1)
		width -= 1;

	if (left_just) {
		if (!buf_writec(buf, rune, 1))
			return false;
		return write_pad(buf_obj, width, false);
	}

	if (!write_pad(buf_obj, width, false))
		return false;
	return buf_writec(buf, rune, 1);
}

static bool fmt_d(tro_dybuffer_obj buf_obj, const opts_d *opts)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);
	bool (*const buf_writes_DYN)(void *, const void *, size_t) =
	    buf_get_writes_DYN(buf_obj);

	const int64_t num      = opts->sint;
	const bool left_just   = opts->left_justified;
	const bool zero_pad    = opts->zero_pad;
	const bool sign_always = opts->sign_always;

	size_t width = opts->width;

	uint8_t genbuf[(TRO_UINT_CHAR_MAX + 1) * sizeof(tro_char16)];
	void *str = genbuf;
	size_t strl;

	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		strl = tro_int2str(num, str, TRO_INT_CHAR_MAX + 1);
		break;
	case TRO_DYBUF_PREF_U16:
		strl = tro_int2str16(num, str, TRO_INT_CHAR_MAX + 1);
		break;
	default:
		strl = tro_int2str_T(num, str, TRO_INT_CHAR_MAX + 1);
		break;
	}

	const bool plus_sign = sign_always && num >= 0;
	if (plus_sign) {
		if (!buf_writec(buf, '+', 1))
			return false;
		if (width >= 1)
			width--;
	}

	if (width > 0) {
		const size_t justw = MAX(width, strl) - strl;

		if (left_just) {
			if (!buf_writes_DYN(buf, str, strl))
				return false;
			return write_pad(buf_obj, justw, zero_pad);
		} else {
			if (!write_pad(buf_obj, justw, zero_pad))
				return false;
			return buf_writes_DYN(buf, str, strl);
		}
	}

	return buf_writes_DYN(buf, str, strl);
}

static bool fmt_u(tro_dybuffer_obj buf_obj, const opts_oxXu *opts)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);
	bool (*const buf_writes_DYN)(void *, const void *, size_t) =
	    buf_get_writes_DYN(buf_obj);

	const uint64_t num   = opts->uint;
	const bool left_just = opts->left_justified;
	const bool zero_pad  = opts->zero_pad;

	size_t width = opts->width;

	uint8_t genbuf[(TRO_UINT_CHAR_MAX + 1) * sizeof(tro_char16)];
	void *str = genbuf;
	size_t strl;

	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		strl = tro_uint2str(num, str, TRO_UINT_CHAR_MAX + 1);
		break;
	case TRO_DYBUF_PREF_U16:
		strl = tro_uint2str16(num, str, TRO_UINT_CHAR_MAX + 1);
		break;
	default:
		strl = tro_uint2str_T(num, str, TRO_UINT_CHAR_MAX + 1);
		break;
	}

	const bool plus_sign = opts->sign_always;
	if (plus_sign) {
		if (!buf_writec(buf, '+', 1))
			return false;
		if (width >= 1)
			width--;
	}

	if (width > 0) {
		const size_t justw = MAX(width, strl) - strl;

		if (left_just) {
			if (!buf_writes_DYN(buf, str, strl))
				return false;
			return write_pad(buf_obj, justw, zero_pad);
		} else {
			if (!write_pad(buf_obj, justw, zero_pad))
				return false;
			return buf_writes_DYN(buf, str, strl);
		}
	}

	return buf_writes_DYN(buf, str, strl);
}

static bool fmt_f(tro_dybuffer_obj buf_obj, const opts_feEgGaA *opts)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);
	bool (*const buf_writes_DYN)(void *, const void *, size_t) =
	    buf_get_writes_DYN(buf_obj);

	const double num       = opts->dfloat;
	const size_t precision = opts->precision;
	const bool alternative = opts->alternative;
	const bool sign_always = opts->sign_always;
	const bool left_just   = opts->left_justified;
	const bool zero_pad    = opts->zero_pad;

	size_t width = opts->width;

	uint8_t genbuf[(TRO_FLOAT_FIXED_CHAR_MAX + 1) * sizeof(tro_char16)];
	void *str = genbuf;
	size_t strl;

	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		if (alternative)
			strl = tro_float2str_adjustable(
			    num, precision, str, TRO_FLOAT_FIXED_CHAR_MAX + 1);
		else
			strl = tro_float2str_fixed(
			    num, precision, str, TRO_FLOAT_FIXED_CHAR_MAX + 1);
		break;
	case TRO_DYBUF_PREF_U16:
		if (alternative)
			strl = tro_float2str16_adjustable(
			    num, precision, str, TRO_FLOAT_FIXED_CHAR_MAX + 1);
		else
			strl = tro_float2str16_fixed(
			    num, precision, str, TRO_FLOAT_FIXED_CHAR_MAX + 1);
		break;
	default:
		if (alternative)
			strl = tro_float2str_adjustable_T(
			    num, precision, str, TRO_FLOAT_FIXED_CHAR_MAX + 1);
		else
			strl = tro_float2str_fixed_T(
			    num, precision, str, TRO_FLOAT_FIXED_CHAR_MAX + 1);
		break;
	}

	const bool plus_sign  = sign_always && !signbit(num);
	const bool minus_sign = sign_always && signbit(num) && isnan(num);

	if (plus_sign)
		if (!buf_writec(buf, '+', 1))
			return false;
	if (minus_sign)
		if (!buf_writec(buf, '-', 1))
			return false;

	if ((plus_sign || minus_sign) && (width >= 1))
		width--;

	if (width > 0) {
		const size_t justw = MAX(width, strl) - strl;

		if (left_just) {
			if (!buf_writes_DYN(buf, str, strl))
				return false;
			return write_pad(buf_obj, justw, zero_pad);
		} else {
			if (!write_pad(buf_obj, justw, zero_pad))
				return false;
			return buf_writes_DYN(buf, str, strl);
		}
	}

	return buf_writes_DYN(buf, str, strl);
}

static inline bool (*buf_get_writes_DYN(tro_dybuffer_obj buf_obj))(void *,
                                                                   const void *,
                                                                   size_t)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);
	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		return (bool (*)(void *, const void *, size_t))buf_writes;
	case TRO_DYBUF_PREF_U16:
		return (bool (*)(void *, const void *, size_t))buf_writes16;
	default:
		return (bool (*)(void *, const void *, size_t))buf_writes_T;
	}
}
