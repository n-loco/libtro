#include "tro/string.h"

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#include "tro/dybuffer.h"
#include "tro/strconv.h"
#include "fmt_parser.h"
#include "utils/math.h"

#ifdef USE_CHAR16_T
# define CHAR_T char16_t

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

static bool fmt_s(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

static bool fmt_S(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

static bool fmt_c(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

static bool fmt_d(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

static bool fmt_u(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

static bool fmt_f(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

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
		case FMT_SPEC_C:
			if (!fmt_c(buf_obj, &spec, args))
				return false;
			break;
		case FMT_SPEC_S:
			if (!fmt_s(buf_obj, &spec, args))
				return false;
			break;
		case FMT_SPEC_CS:
			if (!fmt_S(buf_obj, &spec, args))
				return false;
			break;
		case FMT_SPEC_D:
			if (!fmt_d(buf_obj, &spec, args))
				return false;
			break;
		case FMT_SPEC_O:
			// TODO!
			if (!buf_writec(buf, u'␀', 1))
				return false;
			break;
		case FMT_SPEC_X:
			// TODO!
			if (!buf_writec(buf, u'␀', 1))
				return false;
			break;
		case FMT_SPEC_CX:
			// TODO!
			if (!buf_writec(buf, u'␀', 1))
				return false;
			break;
		case FMT_SPEC_U:
			if (!fmt_u(buf_obj, &spec, args))
				return false;
			break;
		case FMT_SPEC_F:
			if (!fmt_f(buf_obj, &spec, args))
				return false;
			break;
		case FMT_SPEC_P:
			// TODO!
			if (!buf_writec(buf, u'␀', 1))
				return false;
			break;
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

static bool fmt_s(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	const fmt_spec_pad field = fmt->field_width;

	const char *str   = va_arg(args, const char *);
	const size_t strl = strlen(str);

	const size_t width =
	    field.from_va ? (size_t)va_arg(args, unsigned int) : field.width;

	if (width > 0) {
		const size_t strw    = tro_str8_urune_len(str, strl);
		const size_t justw   = MAX(width, strw) - strw;
		const bool left_just = fmt->flags.left_justified;

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

static bool fmt_S(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	const fmt_spec_pad field = fmt->field_width;

	const char16_t *str = va_arg(args, const char16_t *);
	const size_t strl   = tro_str16len(str);

	const size_t width =
	    field.from_va ? (size_t)va_arg(args, unsigned int) : field.width;

	if (width > 0) {
		const size_t strw    = tro_str16_urune_len(str, strl);
		const size_t justw   = MAX(width, strw) - strw;
		const bool left_just = fmt->flags.left_justified;

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

static bool fmt_c(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	const tro_urune rune     = va_arg(args, tro_urune);
	const fmt_spec_pad field = fmt->field_width;

	const bool alternative = fmt->flags.alternative;
	const bool left_just   = fmt->flags.left_justified;

	size_t width =
	    field.from_va ? (size_t)va_arg(args, unsigned int) : field.width;

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

static inline int64_t get_int_arg(fmt_spec_len len, va_list args);

static bool fmt_d(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);
	bool (*const buf_writes_DYN)(void *, const void *, size_t) =
	    buf_get_writes_DYN(buf_obj);

	const fmt_spec_pad field = fmt->field_width;

	int64_t num = get_int_arg(fmt->len, args);

	uint8_t genbuf[(TRO_UINT_CHAR_MAX + 1) * sizeof(char16_t)];
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

	size_t width =
	    (field.from_va ? (size_t)va_arg(args, unsigned int) : field.width);

	const bool plus_sign = fmt->flags.sign_always && num >= 0;
	if (plus_sign) {
		if (!buf_writec(buf, '+', 1))
			return false;
		if (width >= 1)
			width--;
	}

	if (width > 0) {
		const size_t justw   = MAX(width, strl) - strl;
		const bool left_just = fmt->flags.left_justified;
		const bool zero_pad  = fmt->flags.zero_pad;

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

static inline uint64_t get_uint_arg(fmt_spec_len len, va_list args);

static bool fmt_u(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);
	bool (*const buf_writes_DYN)(void *, const void *, size_t) =
	    buf_get_writes_DYN(buf_obj);

	const fmt_spec_pad field = fmt->field_width;

	uint64_t num = get_uint_arg(fmt->len, args);

	uint8_t genbuf[(TRO_UINT_CHAR_MAX + 1) * sizeof(char16_t)];
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

	size_t width =
	    field.from_va ? (size_t)va_arg(args, unsigned int) : field.width;

	const bool plus_sign = fmt->flags.sign_always;
	if (plus_sign) {
		if (!buf_writec(buf, '+', 1))
			return false;
		if (width >= 1)
			width--;
	}

	if (width > 0) {
		const size_t justw   = MAX(width, strl) - strl;
		const bool left_just = fmt->flags.left_justified;
		const bool zero_pad  = fmt->flags.zero_pad;

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

static inline double get_double_arg(fmt_spec_len len, va_list args);

static bool fmt_f(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);
	bool (*const buf_writes_DYN)(void *, const void *, size_t) =
	    buf_get_writes_DYN(buf_obj);

	const fmt_spec_pad field = fmt->field_width;

	double num = get_double_arg(fmt->len, args);

	uint8_t genbuf[(TRO_FLOAT_FIXED_CHAR_MAX + 1) * sizeof(char16_t)];
	void *str = genbuf;
	size_t strl;

	const fmt_spec_pad precsw = fmt->precision_width;
	const size_t precision =
	    precsw.dot ? precsw.from_va ? (size_t)va_arg(args, unsigned int)
	                                : precsw.width
	               : 6;

	const bool alternative = fmt->flags.alternative;
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

	size_t width =
	    field.from_va ? (size_t)va_arg(args, unsigned int) : field.width;

	const bool plus_sign = fmt->flags.sign_always && !signbit(num);
	const bool minus_sign =
	    fmt->flags.sign_always && signbit(num) && isnan(num);

	if (plus_sign)
		if (!buf_writec(buf, '+', 1))
			return false;
	if (minus_sign)
		if (!buf_writec(buf, '-', 1))
			return false;

	if ((plus_sign || minus_sign) && (width >= 1))
		width--;

	if (width > 0) {
		const size_t justw   = MAX(width, strl) - strl;
		const bool left_just = fmt->flags.left_justified;
		const bool zero_pad  = fmt->flags.zero_pad;

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

static inline int64_t get_int_arg(fmt_spec_len len, va_list args)
{
	switch (len) {
	default:
		return (int64_t)va_arg(args, int);
	case FMT_SPEC_LEN_HH:
		return (int64_t)(signed char)(va_arg(args, unsigned int) &
		                              UCHAR_MAX);
	case FMT_SPEC_LEN_H:
		return (int64_t)(short)(va_arg(args, unsigned int) & USHRT_MAX);
	case FMT_SPEC_LEN_L:
		return (int64_t)va_arg(args, long);
	case FMT_SPEC_LEN_LL:
		return (int64_t)va_arg(args, long long);
	case FMT_SPEC_LEN_J:
		return (int64_t)va_arg(args, intmax_t);
	case FMT_SPEC_LEN_Z:
		// fall through
	case FMT_SPEC_LEN_T:
		return (int64_t)va_arg(args, ptrdiff_t);
	}
}

static inline uint64_t get_uint_arg(fmt_spec_len len, va_list args)
{
	switch (len) {
	default:
		return (uint64_t)va_arg(args, unsigned int);
	case FMT_SPEC_LEN_HH:
		return (uint64_t)(unsigned char)(va_arg(args, unsigned int) &
		                                 UCHAR_MAX);
	case FMT_SPEC_LEN_H:
		return (uint64_t)(unsigned short)(va_arg(args, unsigned int) &
		                                  USHRT_MAX);
	case FMT_SPEC_LEN_L:
		return (uint64_t)va_arg(args, unsigned long);
	case FMT_SPEC_LEN_LL:
		return (uint64_t)va_arg(args, unsigned long long);
	case FMT_SPEC_LEN_J:
		return (uint64_t)va_arg(args, uintmax_t);
	case FMT_SPEC_LEN_Z:
		// fall through
	case FMT_SPEC_LEN_T:
		return (uint64_t)va_arg(args, size_t);
	}
}

static inline double get_double_arg(fmt_spec_len len, va_list args)
{
	if (len == FMT_SPEC_LEN_CL)
		return (double)va_arg(args, long double);

	return va_arg(args, double);
}
