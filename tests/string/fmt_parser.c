#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "string/fmt_parser.h"

static bool impl__fmt_assert(const char *name, const fmt_specifier *ef,
                             const fmt_specifier *gf, size_t er, size_t gr);

#define fmt_assert(name, ef, gf, er, gr)                                       \
	if (!impl__fmt_assert(name, &ef, &gf, er, gr))                         \
		return 1;

static const fmt_specifier XPT_FMT_NONE = {
    .type = FMT_SPEC_NONE,
};

static const fmt_specifier XPT_FMT_ESC = {
    .type = FMT_SPEC_ESC,
};

static const fmt_specifier XPT_FMT_C = {
    .type = FMT_SPEC_C,
};

static const fmt_specifier XPT_FMT_LC = {
    .type = FMT_SPEC_C,
    .len  = FMT_SPEC_LEN_L,
};

static const fmt_specifier XPT_FMT_LLD = {
    .type = FMT_SPEC_D,
    .len  = FMT_SPEC_LEN_LL,
};

static const fmt_specifier XPT_FMT_08LLX = {
    .type        = FMT_SPEC_X,
    .len         = FMT_SPEC_LEN_LL,
    .field_width = {.width = 8},
    .flags       = {.zero_pad = true},
};

static const fmt_specifier XPT_FMT_02HHcX = {
    .type        = FMT_SPEC_CX,
    .len         = FMT_SPEC_LEN_HH,
    .field_width = {.width = 2},
    .flags       = {.zero_pad = true},
};

static const fmt_specifier XPT_FMT_ljvaS = {
    .type        = FMT_SPEC_S,
    .field_width = {.from_va = true},
    .flags       = {.left_justified = true},
};

static const fmt_specifier XPT_FMT_dot12F = {
    .type            = FMT_SPEC_F,
    .precision_width = {.dot = true, .width = 12},
};

static const fmt_specifier XPT_FMT_ERROR = {
    .type = FMT_SPEC_ERROR,
};

int main(void)
{
	fmt_specifier fmt_none;
	size_t fmt_none_r = tro__parse_fmt_spec("ordinary text", &fmt_none);
	fmt_assert("Ordinary text", XPT_FMT_NONE, fmt_none, 1, fmt_none_r);

	fmt_specifier fmt_esc;
	size_t fmt_esc_r = tro__parse_fmt_spec("%%", &fmt_esc);
	fmt_assert("Escape", XPT_FMT_ESC, fmt_esc, 2, fmt_esc_r);

	fmt_specifier fmt_c;
	size_t fmt_c_r = tro__parse_fmt_spec("%c", &fmt_c);
	fmt_assert("c", XPT_FMT_C, fmt_c, 2, fmt_c_r);

	fmt_specifier fmt_lc;
	size_t fmt_lc_r = tro__parse_fmt_spec("%lc", &fmt_lc);
	fmt_assert("lc", XPT_FMT_LC, fmt_lc, 3, fmt_lc_r);

	fmt_specifier fmt_lld;
	size_t fmt_lld_r = tro__parse_fmt_spec("%lld", &fmt_lld);
	fmt_assert("lld", XPT_FMT_LLD, fmt_lld, 4, fmt_lld_r);

	fmt_specifier fmt_08llx;
	size_t fmt_08llx_r = tro__parse_fmt_spec("%08llx", &fmt_08llx);
	fmt_assert("08llx", XPT_FMT_08LLX, fmt_08llx, 6, fmt_08llx_r);

	fmt_specifier fmt_02hhX;
	size_t fmt_02hhX_r = tro__parse_fmt_spec("%02hhX", &fmt_02hhX);
	fmt_assert("02hhX", XPT_FMT_02HHcX, fmt_02hhX, 6, fmt_02hhX_r);

	fmt_specifier fmt_ljvas;
	size_t fmt_ljvas_r = tro__parse_fmt_spec("%-*s", &fmt_ljvas);
	fmt_assert("-*s", XPT_FMT_ljvaS, fmt_ljvas, 4, fmt_ljvas_r);

	fmt_specifier fmt_dot12f;
	size_t fmt_dot12f_r = tro__parse_fmt_spec("%.12f", &fmt_dot12f);
	fmt_assert(".12f", XPT_FMT_dot12F, fmt_dot12f, 5, fmt_dot12f_r);

	fmt_specifier fmt_cut;
	size_t fmt_cut_r = tro__parse_fmt_spec("%j", &fmt_cut);
	fmt_assert("Cut", XPT_FMT_ERROR, fmt_cut, 2, fmt_cut_r);

	fmt_specifier fmt_cut2;
	size_t fmt_cut2_r = tro__parse_fmt_spec("%l", &fmt_cut2);
	fmt_assert("Cut 2", XPT_FMT_ERROR, fmt_cut2, 2, fmt_cut2_r);

	fmt_specifier fmt_invalid;
	size_t fmt_invalid_r = tro__parse_fmt_spec("%lv", &fmt_invalid);
	fmt_assert("Invalid", XPT_FMT_ERROR, fmt_invalid, 2, fmt_invalid_r);
}

static const char *type2s(fmt_spec_type t);
static const char *len2s(fmt_spec_len t);

static bool impl__fmt_assert(const char *name, const fmt_specifier *ef,
                             const fmt_specifier *gf, size_t er, size_t gr)
{
	if (er != gr) {
		fprintf(stderr,
		        "On \"%s\", wrong read len: expected %zu, got %zu\n",
		        name, er, gr);
		return false;
	}

	bool umt_type = ef->type != gf->type;
	bool umt_len  = ef->len != gf->len;
	bool umt_precsw =
	    ef->precision_width.dot != gf->precision_width.dot ||
	    ef->precision_width.from_va != gf->precision_width.from_va ||
	    ef->precision_width.width != gf->precision_width.width;
	bool umt_fieldw = ef->field_width.dot != gf->field_width.dot ||
	                  ef->field_width.from_va != gf->field_width.from_va ||
	                  ef->field_width.width != gf->field_width.width;
	bool umt_flags = ef->flags.sign_always != gf->flags.sign_always ||
	                 ef->flags.left_justified != gf->flags.left_justified ||
	                 ef->flags.alternative != gf->flags.alternative ||
	                 ef->flags.zero_pad != gf->flags.zero_pad;

	bool unmatch =
	    umt_type || umt_len || umt_precsw || umt_fieldw || umt_flags;

	if (unmatch) {
		fprintf(stderr, "On \"%s\", unmatched expectations:\n", name);
		fprintf(stderr, "  expected:\n");

		if (umt_type)
			fprintf(stderr, "           type: %s\n",
			        type2s(ef->type));
		if (umt_len)
			fprintf(stderr, "            len: %s\n",
			        len2s(ef->len));
		if (umt_precsw) {
			fprintf(stderr, "      precision:\n");
			fprintf(stderr, "            dot: %s\n",
			        ef->precision_width.dot ? "true" : "false");
			fprintf(stderr, "        from va: %s\n",
			        ef->precision_width.from_va ? "true" : "false");
			fprintf(stderr, "          width: %d\n",
			        ef->precision_width.width);
		}
		if (umt_fieldw) {
			fprintf(stderr, "    field width:\n");
			fprintf(stderr, "        from va: %s\n",
			        ef->field_width.from_va ? "true" : "false");
			fprintf(stderr, "          width: %d\n",
			        ef->field_width.width);
		}
		if (umt_flags) {
			fprintf(stderr, "          flags:");
			if (ef->flags.sign_always)
				fprintf(stderr, " sign_always");
			if (ef->flags.left_justified)
				fprintf(stderr, " left_justified");
			if (ef->flags.alternative)
				fprintf(stderr, " alternative");
			if (ef->flags.zero_pad)
				fprintf(stderr, " zero_pad");
			fprintf(stderr, "\n");
		}

		fprintf(stderr, "\n");

		fprintf(stderr, "  got:\n");

		if (umt_type)
			fprintf(stderr, "           type: %s\n",
			        type2s(gf->type));
		if (umt_len)
			fprintf(stderr, "            len: %s\n",
			        len2s(gf->len));
		if (umt_precsw) {
			fprintf(stderr, "      precision:\n");
			fprintf(stderr, "            dot: %s\n",
			        gf->precision_width.dot ? "true" : "false");
			fprintf(stderr, "        from va: %s\n",
			        gf->precision_width.from_va ? "true" : "false");
			fprintf(stderr, "          width: %d\n",
			        gf->precision_width.width);
		}
		if (umt_fieldw) {
			fprintf(stderr, "    field width:\n");
			fprintf(stderr, "        from va: %s\n",
			        gf->field_width.from_va ? "true" : "false");
			fprintf(stderr, "          width: %d\n",
			        gf->field_width.width);
		}
		if (umt_flags) {
			fprintf(stderr, "          flags:");
			if (gf->flags.sign_always)
				fprintf(stderr, " sign_always");
			if (gf->flags.left_justified)
				fprintf(stderr, " left_justified");
			if (gf->flags.alternative)
				fprintf(stderr, " alternative");
			if (gf->flags.zero_pad)
				fprintf(stderr, " zero_pad");
			fprintf(stderr, "\n");
		}

		fprintf(stderr, "\n");

		return false;
	}

	return true;
}

#define str_case(c)                                                            \
	case c:                                                                \
		return #c

static const char *type2s(fmt_spec_type t)
{
	switch (t) {
		str_case(FMT_SPEC_NONE);
		str_case(FMT_SPEC_ERROR);
		str_case(FMT_SPEC_ESC);
		str_case(FMT_SPEC_C);
		str_case(FMT_SPEC_S);
		str_case(FMT_SPEC_CS);
		str_case(FMT_SPEC_D);
		str_case(FMT_SPEC_O);
		str_case(FMT_SPEC_X);
		str_case(FMT_SPEC_CX);
		str_case(FMT_SPEC_U);
		str_case(FMT_SPEC_F);
		str_case(FMT_SPEC_P);
	}
	return "";
}

static const char *len2s(fmt_spec_len l)
{
	switch (l) {
		str_case(FMT_SPEC_LEN_NONE);
		str_case(FMT_SPEC_LEN_HH);
		str_case(FMT_SPEC_LEN_H);
		str_case(FMT_SPEC_LEN_L);
		str_case(FMT_SPEC_LEN_LL);
		str_case(FMT_SPEC_LEN_J);
		str_case(FMT_SPEC_LEN_Z);
		str_case(FMT_SPEC_LEN_T);
		str_case(FMT_SPEC_LEN_CL);
	}
	return "";
}
