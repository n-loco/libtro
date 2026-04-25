#include "tro/strconv.h"

#include <stddef.h>
#include <stdint.h>

#include "utils/math.h"

#ifdef USE_CHAR16_T
# include <uchar.h>
# define CHAR_T char16_t
# define tro_int2str_T tro_int2str16
# define tro_uint2str_T tro_uint2str16
#else
# define CHAR_T char
# define tro_int2str_T tro_int2str
# define tro_uint2str_T tro_uint2str
#endif

static inline size_t calc_uint_siz(uint64_t num);

size_t tro_int2str_T(int64_t num, CHAR_T *out, size_t outcap)
{
	if (out != NULL && outcap < 2) {
		if (outcap > 0)
			*out = '\0';
		return 0;
	}

	if (num == 0) {
		if (out != NULL) {
			out[0] = '0';
			out[1] = '\0';
		}
		return 1;
	}

	if (num < 0) {
		if (out != NULL) {
			out[0] = '-';
			if (outcap == 1)
				return 1;
		}

		CHAR_T *uout;
		size_t uoutcap;
		if (out != NULL) {
			uout    = out + 1;
			uoutcap = outcap - 1;
		} else {
			uout    = NULL;
			uoutcap = 0;
		}

		uint64_t unum;
		if (num == INT64_MIN)
			unum = (uint64_t)num;
		else
			unum = num * -1;

		return tro_uint2str_T(unum, uout, uoutcap) + 1;
	}

	return tro_uint2str_T(num, out, outcap);
}

size_t tro_uint2str_T(uint64_t num, CHAR_T *out, size_t outcap)
{
	if (out != NULL && outcap < 2) {
		if (outcap > 0)
			*out = '\0';
		return 0;
	}

	if (num == 0) {
		if (out != NULL) {
			out[0] = '0';
			out[1] = '\0';
		}
		return 1;
	}

	const size_t dcount = calc_uint_siz(num);

	if (out == NULL)
		return dcount;

	const size_t outcap_bn = outcap - 1;

	size_t wout = 0;
	while (num != 0) {
		/*
		   Como este algoritmo converte os digitos
		   da direita para a esquerda, precisamos
		   colocá-los na string da mesma forma.
		*/
		const size_t cpos = MIN(dcount - 1, outcap_bn) - wout;

		out[cpos] = (num % 10) + '0';
		wout++;

		num /= 10;

		if (wout == outcap_bn)
			break;
	}

	out[wout] = '\0';
	return wout;
}

static inline size_t calc_uint_siz(uint64_t num)
{
	/*
	   Sim é de conhecimento que poderia-se
	   usar um loop para realizar esse trabalho,
	   porém assim evitamos fazer a divisão que
	   seria necessária em um algoritmo com loop.
	*/
	if (num >= 10000000000000000000ULL)
		return 20;
	else if (num >= 1000000000000000000ULL)
		return 19;
	else if (num >= 100000000000000000ULL)
		return 18;
	else if (num >= 10000000000000000ULL)
		return 17;
	else if (num >= 1000000000000000ULL)
		return 16;
	else if (num >= 100000000000000ULL)
		return 15;
	else if (num >= 10000000000000ULL)
		return 14;
	else if (num >= 1000000000000ULL)
		return 13;
	else if (num >= 100000000000ULL)
		return 12;
	else if (num >= 10000000000ULL)
		return 11;
	else if (num >= 1000000000ULL)
		return 10;
	else if (num >= 100000000ULL)
		return 9;
	else if (num >= 10000000ULL)
		return 8;
	else if (num >= 1000000ULL)
		return 7;
	else if (num >= 100000ULL)
		return 6;
	else if (num >= 10000ULL)
		return 5;
	else if (num >= 1000ULL)
		return 4;
	else if (num >= 100ULL)
		return 3;
	else if (num >= 10ULL)
		return 2;
	else // num >= 0
		return 1;
}
