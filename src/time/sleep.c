#include "tro/time.h"
#include "tro/conditionals.h"

#include <stdint.h>

#if TRO_SYSTEM_WIN32
# include <synchapi.h>
#elif TRO_SYSTEM_UNIX_LIKE
# include <unistd.h>
# include <time.h>
#endif

void tro_sleep(uint32_t interval)
{
#if TRO_SYSTEM_WIN32
	Sleep(interval * 1000);
#elif TRO_SYSTEM_UNIX_LIKE
	sleep(interval);
#endif
}

void tro_msleep(uint32_t interval)
{
#if TRO_SYSTEM_WIN32
	Sleep(interval);
#elif TRO_SYSTEM_UNIX_LIKE
	struct timespec t = {0};
	if (interval >= 1000) {
		t.tv_sec  = interval / 1000;
		t.tv_nsec = (interval % 1000) * 1000000;
	} else
		t.tv_nsec = interval * 1000000;

	nanosleep(&t, NULL);
#endif
}
