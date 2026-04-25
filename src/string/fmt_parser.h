#ifndef FMT_PARSER_H
#define FMT_PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include <uchar.h>

typedef enum {
	FMT_SPEC_NONE  = 0,
	FMT_SPEC_ERROR = 1,
	FMT_SPEC_ESC   = '%',

	FMT_SPEC_C  = 'c',
	FMT_SPEC_S  = 's',
	FMT_SPEC_CS = 'S',
	FMT_SPEC_D  = 'd',
	FMT_SPEC_O  = 'o',
	FMT_SPEC_X  = 'x',
	FMT_SPEC_CX = 'X',
	FMT_SPEC_U  = 'u',
	FMT_SPEC_F  = 'f',
	FMT_SPEC_P  = 'p',
} fmt_spec_type;

typedef enum {
	FMT_SPEC_LEN_NONE = 0,

	FMT_SPEC_LEN_HH = 0x6868, // "hh"
	FMT_SPEC_LEN_H  = 'h',
	FMT_SPEC_LEN_L  = 'l',
	FMT_SPEC_LEN_LL = 0x6C6C, // "ll"
	FMT_SPEC_LEN_J  = 'j',
	FMT_SPEC_LEN_Z  = 'z',
	FMT_SPEC_LEN_T  = 't',
	FMT_SPEC_LEN_CL = 'L',
} fmt_spec_len;

typedef struct {
	bool left_justified: 1;
	bool sign_always   : 1;
	bool alternative   : 1;
	bool zero_pad      : 1;
} fmt_spec_flags;

typedef struct {
	int width;
	bool from_va: 1;
	bool dot    : 1;
} fmt_spec_pad;

typedef struct {
	fmt_spec_pad field_width;
	fmt_spec_pad precision_width;
	fmt_spec_type type;
	fmt_spec_len len;
	fmt_spec_flags flags;
} fmt_specifier;

size_t tro__parse_fmt_spec(const char *sspec, fmt_specifier *out);
size_t tro__parse_fmt16_spec(const char16_t *sspec, fmt_specifier *out);

#endif // FMT_PARSER_H
