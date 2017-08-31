#ifndef MRT_DTYPE_H
#define MRT_DTYPE_H

#include <limits.h>
#ifdef WIN32
#include <float.h>
#endif

#if defined(__sparc__) || defined(__sparc)
# if defined __arch64__ || defined __sparcv9 || defined __sparc_v9__ 
#  define MRT_OS_SPARC64
# endif
# define MRT_OS_SPARC
#endif

#if defined(__sgi__) || defined(sgi) || defined(__sgi)
# define MRT_OS_SGI
#endif

#if defined(__LP64__) || defined(__powerpc64__) || defined MRT_OS_SPARC64
typedef long               MRT_INT64;
typedef unsigned long      MRT_UINT64;
typedef long *             MRT_INT64_PTR;
typedef unsigned long *    MRT_UINT64_PTR;
#define MRT_INT64_FMT      "%ld"
#define MRT_UINT64_FMT     "%lu"
#elif defined(__GNUC__) || defined(_LONG_LONG) || defined(MRT_OS_SGI) || defined(__MWERKS__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined (__APPLE_CC__) || defined(_CRAYC)
typedef long long            MRT_INT64;
typedef unsigned long long   MRT_UINT64;
typedef long long *          MRT_INT64_PTR;
typedef unsigned long long * MRT_UINT64_PTR;
#define MRT_INT64_FMT        "%lld"
#define MRT_UINT64_FMT       "%llu"
#elif defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || (defined(__alpha) && defined (__DECC))
typedef __int64            MRT_INT64;
typedef unsigned __int64   MRT_UINT64;
typedef __int64 *          MRT_INT64_PTR;
typedef unsigned __int64 * MRT_UINT64_PTR;
#define MRT_INT64_FMT      "%I64d"
#define MRT_UINT64_FMT     "%I64u"
#elif
#error 64-bit data type not found!
#endif

#if defined(__LP64__) || defined(__powerpc64__) || defined(MRT_OS_SPARC64) || defined(__APPLE__)
# define MRT_SIZE_T_FMT  "%lu"
# define MRT_SIZE_T_PRFX "lu"
#else
# if defined(__CYGWIN__) || defined(__APPLE__)
#  define MRT_SIZE_T_FMT  "%d"
#  define MRT_SIZE_T_PRFX "d"
# else
#  define MRT_SIZE_T_FMT  "%u"
#  define MRT_SIZE_T_PRFX "u"
# endif
#endif

#if CHAR_BITS > 8
# error Sorry, non-8-bit char data types not supported!
#endif

typedef char             MRT_INT8;
typedef unsigned char    MRT_UINT8;
typedef char *           MRT_INT8_PTR;
typedef unsigned char *  MRT_UINT8_PTR;

typedef float            MRT_FLOAT4;
typedef double           MRT_FLOAT8;
typedef float *          MRT_FLOAT4_PTR;
typedef double *         MRT_FLOAT8_PTR;

#if (USHRT_MAX == 65535) 
typedef short            MRT_INT16;
typedef unsigned short   MRT_UINT16;
typedef short *          MRT_INT16_PTR;
typedef unsigned short * MRT_UINT16_PTR;
#else
#error 16-bit data type not found!
#endif

#if (INT_MAX == 2147483647)
typedef int             MRT_INT32;
typedef unsigned int    MRT_UINT32;
typedef int *           MRT_INT32_PTR;
typedef unsigned int *  MRT_UINT32_PTR;
#define MRT_INT32_FMT   "%d"
#define MRT_UINT32_FMT  "%u"
#elif (LONG_MAX == 2147483647)
typedef long            MRT_INT32;
typedef unsigned long   MRT_UINT32;
typedef long *          MRT_INT32_PTR;
typedef unsigned long * MRT_UINT32_PTR;
#define MRT_INT32_FMT   "%ld"
#define MRT_UINT32_FMT  "%lu"
#else
#error 32-bit data type not found!
#endif

#define MRT_INT8_MAX       127
#define MRT_INT8_MIN       (-MRT_INT8_MAX - 1)
#define MRT_UINT8_MAX      255

#define MRT_INT16_MAX      32767
#define MRT_INT16_MIN      (-MRT_INT16_MAX - 1)
#define MRT_UINT16_MAX     65535

#define MRT_INT32_MAX      2147483647
#define MRT_INT32_MIN      (-MRT_INT32_MAX - 1)
#define MRT_UINT32_MAX     4294967295U

#define MRT_INT64_MAX      9223372036854775807LL
#define MRT_INT64_MIN      (-MRT_INT64_MAX - 1LL)
#define MRT_UINT64_MAX     18446744073709551615ULL

#if defined(__CYGWIN__) || defined(__APPLE__)
#define MRT_FLOAT4_MAX     MAXFLOAT
#define MRT_FLOAT4_MIN     MINFLOAT
#else
#define MRT_FLOAT4_MAX     FLT_MAX
#define MRT_FLOAT4_MIN     FLT_MIN
#endif

#endif

