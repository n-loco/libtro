#include "tro/string.h"

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdarg.h>

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
#else
# define CHAR_T char

# define tro_sfmt_T tro_sfmt
# define tro_vsfmt_T tro_vsfmt

# define buf_writes_T buf_writes

# define tro__parse_fmt_spec_T tro__parse_fmt_spec
# define tro_int2str_T tro_int2str
# define tro_uint2str_T tro_uint2str
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

static bool fmt_d(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

static bool fmt_u(tro_dybuffer_obj buf, const fmt_specifier *fmt, va_list args);

TRO__API bool tro_sfmt_T(tro_dybuffer_obj buf, const CHAR_T *format, ...)
{
	va_list args;
	va_start(args, format);
	bool r = tro_vsfmt_T(buf, format, args);
	va_end(args);
	return r;
}

TRO__API bool tro_vsfmt_T(tro_dybuffer_obj buf_obj, const CHAR_T *format,
                          va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);

	write_text_state text_state = {.text = format};

	size_t i = 0;
	for (;;) {
		fmt_specifier spec;
		const size_t specl = tro__parse_fmt_spec_T(format + i, &spec);
		text_state.is_text = spec.type == FMT_SPEC_NONE;

		text_state.index = i;
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
			const tro_urune rune      = va_arg(args, tro_urune);
			const fmt_spec_pad fieldw = spec.field_width;

			size_t just = fieldw.from_va
			                  ? (size_t)va_arg(args, unsigned int)
			                  : fieldw.width;
			if (just > 1)
				just -= 1;
			else
				just = 1;

			if (spec.flags.left_justified) {
				if (!buf_writec(buf, rune, 1))
					return false;
				if (!write_pad(buf_obj, just, false))
					return false;
			} else {
				if (!write_pad(buf_obj, just, false))
					return false;
				if (!buf_writec(buf, rune, 1))
					return false;
			}
		} break;
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
			// TODO!
			if (!buf_writec(buf, u'␀', 1))
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

	const fmt_spec_pad fieldw = fmt->field_width;

	const char *str   = va_arg(args, const char *);
	const size_t strl = strlen(str);

	const size_t fieldw_w =
	    fieldw.from_va ? (size_t)va_arg(args, unsigned int) : fieldw.width;

	if (fieldw_w > 0) {
		const size_t strw    = tro_str8_urune_len(str, strl);
		const size_t justw   = MAX(fieldw_w, strw) - strw;
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

	const fmt_spec_pad fieldw = fmt->field_width;

	const char16_t *str = va_arg(args, const char16_t *);
	const size_t strl   = tro_str16len(str);

	const size_t fieldw_w =
	    fieldw.from_va ? (size_t)va_arg(args, unsigned int) : fieldw.width;

	if (fieldw_w > 0) {
		const size_t strw    = tro_str16_urune_len(str, strl);
		const size_t justw   = MAX(fieldw_w, strw) - strw;
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

static bool fmt_d(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);

	bool (*buf_writes_DYN)(void *, const void *, size_t);
	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		buf_writes_DYN =
		    (bool (*)(void *, const void *, size_t))buf_writes;
		break;
	case TRO_DYBUF_PREF_U16:
		buf_writes_DYN =
		    (bool (*)(void *, const void *, size_t))buf_writes16;
		break;
	default:
		buf_writes_DYN =
		    (bool (*)(void *, const void *, size_t))buf_writes_T;
		break;
	}

	const fmt_spec_pad fieldw = fmt->field_width;

	int64_t d;
	switch (fmt->len) {
	default:
		d = (int64_t)va_arg(args, int);
		break;
	case FMT_SPEC_LEN_HH:
		d = (int64_t)(signed char)(va_arg(args, unsigned int) &
		                           UCHAR_MAX);
		break;
	case FMT_SPEC_LEN_H:
		d = (int64_t)(short)(va_arg(args, unsigned int) & USHRT_MAX);
		break;
	case FMT_SPEC_LEN_L:
		d = (int64_t)va_arg(args, long);
		break;
	case FMT_SPEC_LEN_LL:
		d = (int64_t)va_arg(args, long long);
		break;
	case FMT_SPEC_LEN_J:
		d = (int64_t)va_arg(args, intmax_t);
		break;
	case FMT_SPEC_LEN_Z:
		// fall through
	case FMT_SPEC_LEN_T:
		d = (int64_t)va_arg(args, ptrdiff_t);
		break;
	}

	union {
		char u8[TRO_INT_CHAR_MAX + 1];
		char16_t u16[TRO_INT_CHAR_MAX + 1];
		CHAR_T uT[TRO_INT_CHAR_MAX + 1];
	} str_u;
	void *str;

	size_t strl;
	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		str  = str_u.u8;
		strl = tro_int2str(d, str_u.u8, TRO_INT_CHAR_MAX + 1);
		break;
	case TRO_DYBUF_PREF_U16:
		str  = str_u.u16;
		strl = tro_int2str16(d, str_u.u16, TRO_INT_CHAR_MAX + 1);
		break;
	default:
		str  = str_u.uT;
		strl = tro_int2str_T(d, str_u.uT, TRO_INT_CHAR_MAX + 1);
		break;
	}

	const size_t fieldw_w =
	    fieldw.from_va ? (size_t)va_arg(args, unsigned int) : fieldw.width;

	if (fieldw_w > 0) {
		const size_t justw   = MAX(fieldw_w, strl) - strl;
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

static bool fmt_u(tro_dybuffer_obj buf_obj, const fmt_specifier *fmt,
                  va_list args)
{
	tro_dispatch_dybuffer(buf_obj, buf);
	const tro_dybuf_pref buf_pref = buf_preference(buf);

	bool (*buf_writes_DYN)(void *, const void *, size_t);
	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		buf_writes_DYN =
		    (bool (*)(void *, const void *, size_t))buf_writes;
		break;
	case TRO_DYBUF_PREF_U16:
		buf_writes_DYN =
		    (bool (*)(void *, const void *, size_t))buf_writes16;
		break;
	default:
		buf_writes_DYN =
		    (bool (*)(void *, const void *, size_t))buf_writes_T;
		break;
	}

	const fmt_spec_pad fieldw = fmt->field_width;

	uint64_t u;
	switch (fmt->len) {
	default:
		u = (uint64_t)va_arg(args, unsigned int);
		break;
	case FMT_SPEC_LEN_HH:
		u = (uint64_t)(unsigned char)(va_arg(args, unsigned int) &
		                              UCHAR_MAX);
		break;
	case FMT_SPEC_LEN_H:
		u = (uint64_t)(unsigned short)(va_arg(args, unsigned int) &
		                               USHRT_MAX);
		break;
	case FMT_SPEC_LEN_L:
		u = (uint64_t)va_arg(args, unsigned long);
		break;
	case FMT_SPEC_LEN_LL:
		u = (uint64_t)va_arg(args, unsigned long long);
		break;
	case FMT_SPEC_LEN_J:
		u = (uint64_t)va_arg(args, uintmax_t);
		break;
	case FMT_SPEC_LEN_Z:
		// fall through
	case FMT_SPEC_LEN_T:
		u = (uint64_t)va_arg(args, size_t);
		break;
	}

	union {
		char u8[TRO_INT_CHAR_MAX + 1];
		char16_t u16[TRO_INT_CHAR_MAX + 1];
		CHAR_T uT[TRO_INT_CHAR_MAX + 1];
	} str_u;
	void *str;

	size_t strl;
	switch (buf_pref) {
	case TRO_DYBUF_PREF_U8:
		str  = str_u.u8;
		strl = tro_uint2str(u, str_u.u8, TRO_INT_CHAR_MAX + 1);
		break;
	case TRO_DYBUF_PREF_U16:
		str  = str_u.u16;
		strl = tro_uint2str16(u, str_u.u16, TRO_INT_CHAR_MAX + 1);
		break;
	default:
		str  = str_u.uT;
		strl = tro_uint2str_T(u, str_u.uT, TRO_INT_CHAR_MAX + 1);
		break;
	}

	const size_t fieldw_w =
	    fieldw.from_va ? (size_t)va_arg(args, unsigned int) : fieldw.width;

	if (fieldw_w > 0) {
		const size_t justw   = MAX(fieldw_w, strl) - strl;
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
