/*
 * CRT definitions
 *
 * Copyright 2000 Francois Gouget.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_CORECRT_H
#define __WINE_CORECRT_H

#include "wine/winheader_enter.h"
#include "wine/32on64utils.h"

#ifndef __WINE_USE_MSVCRT
#define __WINE_USE_MSVCRT
#endif

#ifdef __WINE_CONFIG_H
# error You cannot use config.h with msvcrt
#endif

#ifndef _WIN32
# define _WIN32
#endif

#ifndef WIN32
# define WIN32
#endif

#if ((defined(__x86_64__) && !defined(__i386_on_x86_64__)) || defined(__powerpc64__) || defined(__aarch64__)) && !defined(_WIN64)
#define _WIN64
#endif

#if !defined(_MSC_VER) && !defined(__int32)
# define __int32 int
#endif

#ifndef _MSVCR_VER
# define _MSVCR_VER 140
#endif

#if !defined(_UCRT) && _MSVCR_VER >= 140
# define _UCRT
#endif

#include <sal.h>

#ifndef _MSC_VER
#  ifndef __int8
#    define __int8  char
#  endif
#  ifndef __int16
#    define __int16 short
#  endif
#  ifndef __int32
#    define __int32 int
#  endif
#  ifndef __int64
#    if (defined(_WIN64) || defined(__i386_on_x86_64__)) && !defined(__MINGW64__)
#      define __int64 long
#    else
#      define __int64 long long
#    endif
#  endif
#endif

#if !defined(_MSC_VER) && !defined(__int3264)
# if defined(_WIN64)
#  define __int3264 __int64
# else
#  define __int3264 __int32
# endif
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL  0
#else
#define NULL  ((void *)0)
#endif
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef _MSC_VER
# undef __stdcall
# ifdef __i386__
#  ifdef __GNUC__
#   if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2)) || defined(__APPLE__)
#    define __stdcall __attribute__((__stdcall__)) __attribute__((__force_align_arg_pointer__))
#   else
#    define __stdcall __attribute__((__stdcall__))
#   endif
#  else
#   error You need to define __stdcall for your compiler
#  endif
# elif defined(__i386_on_x86_64__)
#   define __stdcall __attribute__((stdcall32)) __attribute__((__force_align_arg_pointer__))
# elif defined(__x86_64__) && defined (__GNUC__)
#  if __has_attribute(__force_align_arg_pointer__)
#   define __stdcall __attribute__((ms_abi)) __attribute__((__force_align_arg_pointer__))
#  else
#   define __stdcall __attribute__((ms_abi))
#  endif
# elif defined(__arm__) && defined (__GNUC__) && !defined(__SOFTFP__) && !defined(__MINGW32__) && !defined(__CYGWIN__)
#   define __stdcall __attribute__((pcs("aapcs-vfp")))
# elif defined(__aarch64__) && defined (__GNUC__) && __has_attribute(ms_abi)
#  define __stdcall __attribute__((ms_abi))
# else  /* __i386__ */
#  define __stdcall
# endif  /* __i386__ */
#endif /* __stdcall */

#ifndef _MSC_VER
# undef __cdecl
# if defined(__i386__) && defined(__GNUC__)
#  if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2)) || defined(__APPLE__)
#   define __cdecl __attribute__((__cdecl__)) __attribute__((__force_align_arg_pointer__))
#  else
#   define __cdecl __attribute__((__cdecl__))
#  endif
# elif defined(__i386_on_x86_64__)
#   define __cdecl __attribute__((cdecl32)) __attribute__((__force_align_arg_pointer__))
# else
#  define __cdecl __stdcall
# endif
#endif

#if defined(__i386_on_x86_64__)
#  define __ms_va_list __builtin_va_list32
#  define __ms_va_start(list,arg) __builtin_va_start32(list,arg)
#  define __ms_va_end(list) __builtin_va_end32(list)
#  define __ms_va_copy(dest,src) __builtin_va_copy32(dest,src)
# elif (defined(__x86_64__) || (defined(__aarch64__) && __has_attribute(ms_abi))) && defined (__GNUC__)
#  define __ms_va_list __builtin_ms_va_list
#  define __ms_va_start(list,arg) __builtin_ms_va_start(list,arg)
#  define __ms_va_end(list) __builtin_ms_va_end(list)
#  define __ms_va_copy(dest,src) __builtin_ms_va_copy(dest,src)
# else
#  define __ms_va_list va_list
#  define __ms_va_start(list,arg) va_start(list,arg)
#  define __ms_va_end(list) va_end(list)
#  ifdef va_copy
#   define __ms_va_copy(dest,src) va_copy(dest,src)
#  else
#   define __ms_va_copy(dest,src) ((dest) = (src))
#  endif
#endif

#ifndef WINAPIV
# if defined(__arm__) && defined (__GNUC__) && !defined(__SOFTFP__) && !defined(__MINGW32__) && !defined(__CYGWIN__)
#  define WINAPIV __attribute__((pcs("aapcs")))
# else
#  define WINAPIV __cdecl
# endif
#endif

#ifndef DECLSPEC_NORETURN
# if defined(_MSC_VER) && (_MSC_VER >= 1200) && !defined(MIDL_PASS)
#  define DECLSPEC_NORETURN __declspec(noreturn)
# elif defined(__GNUC__)
#  define DECLSPEC_NORETURN __attribute__((noreturn))
# else
#  define DECLSPEC_NORETURN
# endif
#endif

#ifndef DECLSPEC_ALIGN
# if defined(_MSC_VER) && (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#  define DECLSPEC_ALIGN(x) __declspec(align(x))
# elif defined(__GNUC__)
#  define DECLSPEC_ALIGN(x) __attribute__((aligned(x)))
# else
#  define DECLSPEC_ALIGN(x)
# endif
#endif

#ifndef _ACRTIMP
# ifdef _CRTIMP
#  define _ACRTIMP _CRTIMP
# elif defined(_MSC_VER)
#  define _ACRTIMP __declspec(dllimport)
# elif defined(__MINGW32__) || defined(__CYGWIN__)
#  define _ACRTIMP __attribute__((dllimport))
# else
#  define _ACRTIMP
# endif
#endif

#define _ARGMAX 100
#define _CRT_INT_MAX 0x7fffffff

#ifndef _MSVCRT_LONG_DEFINED
#define _MSVCRT_LONG_DEFINED
/* we need 32-bit longs even on 64-bit */
#ifdef __LP64__
typedef int __msvcrt_long;
typedef unsigned int __msvcrt_ulong;
#else
typedef long __msvcrt_long;
typedef unsigned long __msvcrt_ulong;
#endif
#endif

#ifndef _INTPTR_T_DEFINED
#ifdef  _WIN64
typedef __int64 intptr_t;
#else
typedef int intptr_t;
#endif
#define _INTPTR_T_DEFINED
#endif

#ifndef _UINTPTR_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64 uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif

#ifndef _PTRDIFF_T_DEFINED
#ifdef _WIN64
typedef __int64 ptrdiff_t;
#else
typedef int ptrdiff_t;
#endif
#define _PTRDIFF_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
#ifdef _WIN64
typedef unsigned __int64 size_t;
#else
typedef unsigned int size_t;
#endif
#define _SIZE_T_DEFINED
#endif

#ifndef _TIME32_T_DEFINED
typedef __msvcrt_long __time32_t;
#define _TIME32_T_DEFINED
#endif

#ifndef _TIME64_T_DEFINED
typedef __int64 DECLSPEC_ALIGN(8) __time64_t;
#define _TIME64_T_DEFINED
#endif

#ifdef _USE_32BIT_TIME_T
# ifdef _WIN64
#  error You cannot use 32-bit time_t in Win64
# endif
#elif !defined(_WIN64)
# define _USE_32BIT_TIME_T
#endif

#ifndef _TIME_T_DEFINED
#ifdef _USE_32BIT_TIME_T
typedef __time32_t time_t;
#else
typedef __time64_t time_t;
#endif
#define _TIME_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif
#define _WCHAR_T_DEFINED
#endif

#ifndef _WCTYPE_T_DEFINED
typedef unsigned short  wint_t;
typedef unsigned short  wctype_t;
#define _WCTYPE_T_DEFINED
#endif

#ifndef _ERRNO_T_DEFINED
typedef int errno_t;
#define _ERRNO_T_DEFINED
#endif

struct threadlocaleinfostruct;
struct threadmbcinfostruct;
typedef struct threadlocaleinfostruct *pthreadlocinfo;
typedef struct threadmbcinfostruct *pthreadmbcinfo;

typedef struct localeinfo_struct
{
    pthreadlocinfo locinfo;
    pthreadmbcinfo mbcinfo;
} _locale_tstruct, *_locale_t;

#ifndef _TAGLC_ID_DEFINED
typedef struct tagLC_ID {
    unsigned short wLanguage;
    unsigned short wCountry;
    unsigned short wCodePage;
} LC_ID, *LPLC_ID;
#define _TAGLC_ID_DEFINED
#endif

#ifndef _THREADLOCALEINFO
typedef struct threadlocaleinfostruct {
#if _MSVCR_VER >= 140
    unsigned short *pctype;
    int mb_cur_max;
    unsigned int lc_codepage;
#endif

    int refcount;
#if _MSVCR_VER < 140
    unsigned int lc_codepage;
#endif
    unsigned int lc_collate_cp;
    __msvcrt_ulong lc_handle[6];
    LC_ID lc_id[6];
    struct {
        char *locale;
        wchar_t *wlocale;
        int *refcount;
        int *wrefcount;
    } lc_category[6];
    int lc_clike;
#if _MSVCR_VER < 140
    int mb_cur_max;
#endif
    int *lconv_intl_refcount;
    int *lconv_num_refcount;
    int *lconv_mon_refcount;
    struct lconv *lconv;
    int *ctype1_refcount;
    unsigned short *ctype1;
#if _MSVCR_VER < 140
    unsigned short *pctype;
#endif
    const unsigned char *pclmap;
    const unsigned char *pcumap;
    struct __lc_time_data *lc_time_curr;
#if _MSVCR_VER >= 110
    wchar_t *lc_name[6];
#endif
} threadlocinfo;
#define _THREADLOCALEINFO
#endif

#if !defined(__WINE_USE_MSVCRT) || defined(__MINGW32__)
#define __WINE_CRT_PRINTF_ATTR(fmt,args) __attribute__((format (printf,fmt,args)))
#define __WINE_CRT_SCANF_ATTR(fmt,args)  __attribute__((format (scanf,fmt,args)))
#else
#define __WINE_CRT_PRINTF_ATTR(fmt,args)
#define __WINE_CRT_SCANF_ATTR(fmt,args)
#endif

#endif /* __WINE_CORECRT_H */
