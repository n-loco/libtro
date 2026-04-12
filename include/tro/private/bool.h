#ifndef TRO__BOOL_H_
#define TRO__BOOL_H_

/*
   Uma condicional especial para
   saber se para usar `bool` é
   necessário incluír `stdbool.h`
   ou não, já que `bool` existe
   em C++ e em C23.
 */
#define TRO__USE_STDBOOL 0

// clang-format off
#if (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L) && !defined(__cplusplus)
# undef TRO__USE_STDBOOL
# define TRO__USE_STDBOOL 1
#endif
// clang-format on

#endif // TRO__BOOL_H_
