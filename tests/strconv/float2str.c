#include <stddef.h>

#include <tro/strconv.h>

#include "utils.h"

#define GENBUF (TRO_FLOAT_FIXED_CHAR_MAX + 1)
static char genbuf[GENBUF];

static const char EXP_f0[]           = "0";
static const char EXP_f0dot00[]      = "0.00";
static const char EXP_f0E0[]         = "0e0";
static const char EXP_f0cE0[]        = "0E0";
static const char EXP_f0dot00Ep00[]  = "0.00e+00";
static const char EXP_f0dot00cEp00[] = "0.00E+00";

static const char EXP_f0dot0035[]     = "0.0035";
static const char EXP_f0dot00350000[] = "0.00350000";
static const char EXP_f0dot004[]      = "0.004";
static const char EXP_f3dot5En3[]     = "3.5e-3";
static const char EXP_f3dot5cEn3[]    = "3.5E-3";
static const char EXP_f3dot50En03[]   = "3.50e-03";
static const char EXP_f3dot50cEn03[]  = "3.50E-03";

static const char EXP_fn0dot0035[]     = "-0.0035";
static const char EXP_fn0dot00350000[] = "-0.00350000";
static const char EXP_fn0dot004[]      = "-0.004";
static const char EXP_fn3dot5En3[]     = "-3.5e-3";
static const char EXP_fn3dot5cEn3[]    = "-3.5E-3";
static const char EXP_fn3dot50En03[]   = "-3.50e-03";
static const char EXP_fn3dot50cEn03[]  = "-3.50E-03";

int main(void)
{
	size_t f0l = tro_float2str(0.0, genbuf, GENBUF);
	str_assert("0", EXP_f0, 1, genbuf, f0l);

	size_t f0dot00l = tro_float2str_fixed(0.0, 2, genbuf, GENBUF);
	str_assert("0.00", EXP_f0dot00, 4, genbuf, f0dot00l);

	size_t f0e0l = tro_float2str_e(0.0, genbuf, GENBUF);
	str_assert("0e0", EXP_f0E0, 3, genbuf, f0e0l);

	size_t f0E0l = tro_float2str_E(0.0, genbuf, GENBUF);
	str_assert("0E0", EXP_f0cE0, 3, genbuf, f0E0l);

	size_t f0dot00ep00l = tro_float2str_fixed_e(0.0, 2, genbuf, GENBUF);
	str_assert("0.00e+00", EXP_f0dot00Ep00, 8, genbuf, f0dot00ep00l);

	size_t f0dot00Ep00l = tro_float2str_fixed_E(0.0, 2, genbuf, GENBUF);
	str_assert("0.00E+00", EXP_f0dot00cEp00, 8, genbuf, f0dot00Ep00l);

	size_t f0dot0035l = tro_float2str(3.5E-3, genbuf, GENBUF);
	str_assert("0.0035", EXP_f0dot0035, 6, genbuf, f0dot0035l);

	size_t f0dot00350000l = tro_float2str_fixed(3.5E-3, 8, genbuf, GENBUF);
	str_assert("0.00350000", EXP_f0dot00350000, 10, genbuf, f0dot00350000l);

	size_t f0dot004l = tro_float2str_adjustable(3.5E-3, 3, genbuf, GENBUF);
	str_assert("0.004", EXP_f0dot004, 5, genbuf, f0dot004l);

	size_t f3dot5en3l = tro_float2str_e(3.5E-3, genbuf, GENBUF);
	str_assert("3.5e-3", EXP_f3dot5En3, 6, genbuf, f3dot5en3l);

	size_t f3dot5En3l = tro_float2str_E(3.5E-3, genbuf, GENBUF);
	str_assert("3.5E-3", EXP_f3dot5cEn3, 6, genbuf, f3dot5En3l);

	size_t f3dot50en03l = tro_float2str_fixed_e(3.5E-3, 2, genbuf, GENBUF);
	str_assert("3.50e-03", EXP_f3dot50En03, 8, genbuf, f3dot50en03l);

	size_t f3dot50En03l = tro_float2str_fixed_E(3.5E-3, 2, genbuf, GENBUF);
	str_assert("3.50E-03", EXP_f3dot50cEn03, 8, genbuf, f3dot50En03l);

	size_t fn0dot0035l = tro_float2str(-3.5E-3, genbuf, GENBUF);
	str_assert("-0.0035", EXP_fn0dot0035, 7, genbuf, fn0dot0035l);

	size_t fn0dot00350000l =
	    tro_float2str_fixed(-3.5E-3, 8, genbuf, GENBUF);
	str_assert("-0.00350000", EXP_fn0dot00350000, 11, genbuf,
	           fn0dot00350000l);

	size_t fn0dot004l =
	    tro_float2str_adjustable(-3.5E-3, 3, genbuf, GENBUF);
	str_assert("-0.004", EXP_fn0dot004, 6, genbuf, fn0dot004l);

	size_t fn3dot5en3l = tro_float2str_e(-3.5E-3, genbuf, GENBUF);
	str_assert("-3.5e-3", EXP_fn3dot5En3, 7, genbuf, fn3dot5en3l);

	size_t fn3dot5En3l = tro_float2str_E(-3.5E-3, genbuf, GENBUF);
	str_assert("-3.5E-3", EXP_fn3dot5cEn3, 7, genbuf, fn3dot5En3l);

	size_t fn3dot50en03l =
	    tro_float2str_fixed_e(-3.5E-3, 2, genbuf, GENBUF);
	str_assert("-3.50e-03", EXP_fn3dot50En03, 9, genbuf, fn3dot50en03l);

	size_t fn3dot50En03l =
	    tro_float2str_fixed_E(-3.5E-3, 2, genbuf, GENBUF);
	str_assert("-3.50E-03", EXP_fn3dot50cEn03, 9, genbuf, fn3dot50En03l);
}
