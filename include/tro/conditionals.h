#ifndef TRO_CONDITIONALS_H_
#define TRO_CONDITIONALS_H_

#if defined(__CYGWIN__) || defined(__MSYS__)
# error "Cygwin and MSYS are not currently supported."
#endif

// ===== Definições das Condicionais ===== //

/**
 * 1 se o compilador for MSVC,
 * 0 caso contrário.
 */
#define TRO_COMPILER_MSVC 0
/**
 * 1 se o compilador for tipo GCC
 * (o próprio GCC, Clang, etc.),
 * 0 caso contrário.
 */
#define TRO_COMPILER_GCC_LIKE 0

/**
 * 1 se o sistema for Win32 (Windows),
 * 0 caso contrário.
 */
#define TRO_SYSTEM_WIN32 0
/**
 * 1 se o sistema for UNIX® ou tipo Unix,
 * 0 caso contrário.
 */
#define TRO_SYSTEM_UNIX_LIKE 0
/**
 * 1 se o sistema for Darwin (macOS e iOS),
 * 0 caso contrário.
 */
#define TRO_SYSTEM_DARWIN 0
/**
 * 1 se o sistema for derivado de Linux
 * (distribuições no geral, como Ubuntu, Fedora ou Android),
 * 0 caso contrário.
 */
#define TRO_SYSTEM_LINUX 0
/**
 * 1 se o sistema for Android,
 * 0 caso contrário.
 *
 * **NOTA:** o motivo desta condicional
 * é para pessoas que usam celulares Android para
 * programar, usando editores de código de
 * celular ou via Termux.
 */
#define TRO_SYSTEM_ANDROID_LINUX 0
/**
 * 1 se o sistema for derivado de Linux que não Android,
 * 0 caso contrário.
 */
#define TRO_SYSTEM_GENERIC_LINUX 0
/**
 * 1 se o sistema for derivado de BSD,
 * 0 caso contrário.
 */
#define TRO_SYSTEM_BSD 0
/**
 * Igual ao `TRO_SYSTEM_BSD`, mas se o
 * sistema for Darwin, também resulta em 1.
 */
#define TRO_SYSTEM_BSD_LIKE 0

// ===== Implementação das Condicionais ===== //

#if defined(_MSC_VER)
# undef TRO_COMPILER_MSVC
# define TRO_COMPILER_MSVC 1
#endif

#if defined(__GNUC__)
# undef TRO_COMPILER_GCC_LIKE
# define TRO_COMPILER_GCC_LIKE 1
#endif

#if defined(_WIN32)
# undef TRO_SYSTEM_WIN32
# define TRO_SYSTEM_WIN32 1
#endif

#if defined(__unix__) || defined(__ANDROID__) || defined(__APPLE__)
# undef TRO_SYSTEM_UNIX_LIKE
# define TRO_SYSTEM_UNIX_LIKE 1
#endif

#if defined(__APPLE__)
# undef TRO_SYSTEM_DARWIN
# define TRO_SYSTEM_DARWIN 1
#endif

#if defined(__linux__)
# undef TRO_SYSTEM_LINUX
# define TRO_SYSTEM_LINUX 1
#endif

#if defined(__ANDROID__)
# undef TRO_SYSTEM_ANDROID_LINUX
# define TRO_SYSTEM_ANDROID_LINUX 1
#elif defined(__linux__)
# undef TRO_SYSTEM_GENERIC_LINUX
# define TRO_SYSTEM_GENERIC_LINUX 1
#endif

// clang-format off
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
# undef TRO_SYSTEM_BSD
# define TRO_SYSTEM_BSD 1
#endif
// clang-format on

#if TRO_SYSTEM_BSD || TRO_SYSTEM_DARWIN
# undef TRO_SYSTEM_BSD_LIKE
# define TRO_SYSTEM_BSD_LIKE 1
#endif

#endif // TRO_CONDITIONALS_H_
