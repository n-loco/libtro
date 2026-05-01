#include "fmt_parser.h"

#include <stddef.h>
#include <stdbool.h>

#ifdef USE_CHAR16_T
# include <uchar.h>
# define CHAR_T char16_t
# define tro__parse_fmt_spec_T tro__parse_fmt16_spec
#else
# define CHAR_T char
# define tro__parse_fmt_spec_T tro__parse_fmt_spec
#endif

static const fmt_specifier no_spec = {
    .type = FMT_SPEC_NONE,
};

static const fmt_specifier err_spec = {
    .type = FMT_SPEC_ERROR,
};

static const fmt_specifier eos_spec = {
    .type = FMT_SPEC_EOS,
};

#define IS_DIGIT(c) ('0' <= c && c <= '9')
#define IS_N0_DIGIT(c) ('1' <= c && c <= '9')

static inline bool parse_flags(const CHAR_T *sspec, fmt_specifier *spec,
                               size_t *read_out);

static inline bool parse_fieldw(const CHAR_T *sspec, fmt_specifier *spec,
                                size_t *read_out);

static inline bool parse_precsw(const CHAR_T *sspec, fmt_specifier *spec,
                                size_t *read_out);

static inline bool parse_len(const CHAR_T *sspec, fmt_specifier *spec,
                             size_t *read_out);

static inline bool parse_type(const CHAR_T *sspec, fmt_specifier *spec);

size_t tro__parse_fmt_spec_T(const CHAR_T *sspec, fmt_specifier *out)
{
	size_t d_read = 0;

	if (sspec == NULL || out == NULL)
		goto ERROR;

	const CHAR_T intro = *sspec;
	if (intro == '\0') {
		*out = eos_spec;
		return 0;
	}

	if (intro != '%') {
		*out = no_spec;
		return 1;
	}
	sspec++;
	d_read++;

	fmt_specifier spec = {0};

	/* Bit flags */ {
		size_t p_read;
		bool success  = parse_flags(sspec, &spec, &p_read);
		sspec        += p_read;
		d_read       += p_read;
		if (!success)
			goto ERROR;
	}
	/* Largura do campo */ {
		size_t p_read;
		bool success  = parse_fieldw(sspec, &spec, &p_read);
		sspec        += p_read;
		d_read       += p_read;
		if (!success)
			goto ERROR;
	}
	/* Precisão */ {
		size_t p_read;
		bool success  = parse_precsw(sspec, &spec, &p_read);
		sspec        += p_read;
		d_read       += p_read;
		if (!success)
			goto ERROR;
	}
	/* Modificador de comprimento */ {
		size_t p_read;
		bool success  = parse_len(sspec, &spec, &p_read);
		sspec        += p_read;
		d_read       += p_read;
		if (!success)
			goto ERROR;
	}

	// Tipo principal
	if (!parse_type(sspec, &spec))
		goto ERROR;

	*out = spec;
	return d_read + 1;

ERROR:
	*out = err_spec;
	return d_read;
}

static inline bool parse_flags(const CHAR_T *sspec, fmt_specifier *spec,
                               size_t *read_out)
{
	size_t read_d = 0;

	for (;;) {
		const CHAR_T f = *sspec;
		if (f == '\0')
			goto ERROR;

		switch (f) {
		case '-':
			spec->flags.left_justified = true;
			break;
		case '+':
			spec->flags.sign_always = true;
			break;
		case '#':
			spec->flags.alternative = true;
			break;
		case '0':
			spec->flags.zero_pad = true;
			break;
		default:
			goto FLAG_FOR_BREAK;
		}

		sspec++;
		read_d++;

		continue;
	FLAG_FOR_BREAK:
		break;
	}

	*read_out = read_d;
	return true;

ERROR:
	*read_out = read_d;
	return false;
}

static inline bool parse_pad(const CHAR_T *sspec, fmt_spec_pad *pad,
                             size_t *read_out)
{
	size_t read_d = 0;

	const CHAR_T c = *sspec;
	if (c == '\0')
		goto ERROR;

	if (c == '*') {
		pad->from_va = true;
		read_d++;
	} else if (IS_N0_DIGIT(c)) {
		pad->width = c - '0';
		sspec++;
		read_d++;

		for (;;) {
			const CHAR_T d = *sspec;
			if (d == '\0')
				goto ERROR;

			if (!IS_DIGIT(d))
				break;

			pad->width *= 10;
			pad->width += d - '0';

			sspec++;
			read_d++;
		}
	}

	*read_out = read_d;
	return true;

ERROR:
	*read_out = read_d;
	return false;
}

static inline bool parse_fieldw(const CHAR_T *sspec, fmt_specifier *spec,
                                size_t *read_out)
{
	const CHAR_T c = *sspec;
	if (c == '\0') {
		*read_out = 0;
		return false;
	}

	if (c == '.') {
		*read_out = 0;
		return true;
	}

	return parse_pad(sspec, &spec->field_width, read_out);
}

static inline bool parse_precsw(const CHAR_T *sspec, fmt_specifier *spec,
                                size_t *read_out)
{
	const CHAR_T c = *sspec;
	if (c == '\0') {
		*read_out = 0;
		return false;
	}
	if (c == '.') {
		spec->precision_width.dot = true;
		sspec++;

		size_t ppad;
		bool success = parse_pad(sspec, &spec->precision_width, &ppad);

		*read_out = ppad + 1;
		return success;
	}

	*read_out = 0;
	return true;
}

static inline bool parse_len(const CHAR_T *sspec, fmt_specifier *spec,
                             size_t *read_out)
{
	size_t read_d = 1;

	const CHAR_T l = sspec[0];
	if (l == '\0')
		goto ERROR;

	switch (l) {
	case 'h': {
		const CHAR_T l2 = sspec[1];
		if (l2 == '\0')
			goto ERROR;

		if (l2 == 'h') {
			spec->len = FMT_SPEC_LEN_HH;
			read_d++;
		} else {
			spec->len = FMT_SPEC_LEN_H;
		}
	} break;
	case 'l': {
		const CHAR_T l2 = sspec[1];
		if (l2 == '\0')
			goto ERROR;

		if (l2 == 'l') {
			spec->len = FMT_SPEC_LEN_LL;
			read_d++;
		} else {
			spec->len = FMT_SPEC_LEN_L;
		}
	} break;
	case 'j':
		spec->len = FMT_SPEC_LEN_J;
		break;
	case 'z':
		spec->len = FMT_SPEC_LEN_Z;
		break;
	case 't':
		spec->len = FMT_SPEC_LEN_T;
		break;
	case 'L':
		spec->len = FMT_SPEC_LEN_CL;
		break;
	default:
		read_d = 0;
	}

	*read_out = read_d;
	return true;

ERROR:
	*read_out = read_d;
	return false;
}

static inline bool parse_type(const CHAR_T *sspec, fmt_specifier *spec)
{
	const CHAR_T st = *sspec;

	switch (st) {
	case '%':
		spec->type = FMT_SPEC_ESC;
		return true;
	case 'c':
		spec->type = FMT_SPEC_C;
		return true;
	case 's':
		spec->type = FMT_SPEC_S;
		return true;
	case 'S':
		spec->type = FMT_SPEC_CS;
		return true;
	case 'd':
		// fall through
	case 'i':
		spec->type = FMT_SPEC_D;
		return true;
	case 'x':
		spec->type = FMT_SPEC_X;
		return true;
	case 'X':
		spec->type = FMT_SPEC_CX;
		return true;
	case 'u':
		spec->type = FMT_SPEC_U;
		return true;
	case 'f':
		spec->type = FMT_SPEC_F;
		return true;
	case 'p':
		spec->type = FMT_SPEC_P;
		return true;
	default:
		return false;
	}
}
