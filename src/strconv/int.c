#include "tro/strconv.h"

#include "utils/math.h"

size_t tro_int2str(int64_t num, char *out, size_t outcap)
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

		char *uout;
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

		return tro_uint2str(unum, uout, uoutcap) + 1;
	}

	return tro_uint2str(num, out, outcap);
}

static size_t calc_uint_siz(uint64_t num);

size_t tro_uint2str(uint64_t num, char *out, size_t outcap)
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

static size_t calc_uint_siz(uint64_t num)
{
	size_t count = 0;
	while (num != 0) {
		num /= 10;
		count++;
	}
	return count;
}
