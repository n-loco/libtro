/** @file */

#ifndef TRO_TIME_H_
#define TRO_TIME_H_

#include "tro/private/api.h"

#include <stdint.h>

TRO__C_API_BEGIN

/**
 * Faz a **thread atual** suspender sua
 * execução por **no mínimo** `interval`.
 *
 * @param interval é o tempo em **segundos**.
 */
TRO__API void tro_sleep(uint32_t interval);

/**
 * Faz a **thread atual** suspender sua
 * execução por **no mínimo** `interval`.
 *
 * @param interval é o tempo em **milissegundos**.
 */
TRO__API void tro_msleep(uint32_t interval);

TRO__C_API_END

#endif // TRO_TIME_H_
