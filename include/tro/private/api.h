#ifndef TRO__API_H_
#define TRO__API_H_

#ifdef __cplusplus
# define TRO__C_API_BEGIN extern "C" {
# define TRO__C_API_END }
#else
# define TRO__C_API_BEGIN
# define TRO__C_API_END
#endif

#define TRO__API extern

#endif // TRO__API_H_
