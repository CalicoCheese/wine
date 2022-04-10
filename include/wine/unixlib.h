/*
 * Definitions for Unix libraries
 *
 * Copyright (C) 2021 Alexandre Julliard
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

#ifndef __WINE_WINE_UNIXLIB_H
#define __WINE_WINE_UNIXLIB_H

#include <wine/winheader_enter.h>

extern USHORT * HOSTPTR uctable DECLSPEC_HIDDEN;
extern USHORT * HOSTPTR lctable DECLSPEC_HIDDEN;

typedef NTSTATUS (*unixlib_entry_t)( void *args );
typedef UINT64 unixlib_handle_t;

extern NTSTATUS WINAPI __wine_unix_call( unixlib_handle_t handle, unsigned int code, void *args );

#ifdef WINE_UNIX_LIB

/* some useful helpers from ntdll */
extern const char *ntdll_get_build_dir(void);
extern const char *ntdll_get_data_dir(void);
extern DWORD ntdll_umbstowcs( const char * HOSTPTR src, DWORD srclen, WCHAR * HOSTPTR dst, DWORD dstlen );
extern int ntdll_wcstoumbs( const WCHAR * HOSTPTR src, DWORD srclen, char * HOSTPTR dst, DWORD dstlen, BOOL strict );
extern NTSTATUS ntdll_init_syscalls( ULONG id, SYSTEM_SERVICE_TABLE *table, void **dispatcher );

NTSTATUS WINAPI KeUserModeCallback( ULONG id, const void *args, ULONG len, void **ret_ptr, ULONG *ret_len );

static inline WCHAR ntdll_towupper( WCHAR ch )
{
    return ch + uctable[uctable[uctable[ch >> 8] + ((ch >> 4) & 0x0f)] + (ch & 0x0f)];
}

static inline WCHAR ntdll_towlower( WCHAR ch )
{
    return ch + lctable[lctable[lctable[ch >> 8] + ((ch >> 4) & 0x0f)] + (ch & 0x0f)];
}

static inline WCHAR *ntdll_wcsupr( WCHAR * str )
{
    WCHAR *ret;
    for (ret = str; *str; str++) *str = ntdll_towupper(*str);
    return ret;
}

/* wide char string functions */

static inline size_t ntdll_wcslen( const WCHAR * HOSTPTR str )
{
    const WCHAR * HOSTPTR s = str;
    while (*s) s++;
    return s - str;
}

/* 32on64 FIXME: If anyone uses the retval we need to make this overloadble. */
static inline WCHAR * HOSTPTR ntdll_wcscpy( WCHAR * HOSTPTR dst, const WCHAR * HOSTPTR src )
{
    WCHAR * HOSTPTR p = dst;
    while ((*p++ = *src++));
    return dst;
}

static inline WCHAR * HOSTPTR ntdll_wcscat( WCHAR * HOSTPTR dst, const WCHAR * HOSTPTR src )
{
    ntdll_wcscpy( dst + ntdll_wcslen(dst), src );
    return dst;
}

static inline int ntdll_wcscmp( const WCHAR * HOSTPTR str1, const WCHAR * HOSTPTR str2 )
{
    while (*str1 && (*str1 == *str2)) { str1++; str2++; }
    return *str1 - *str2;
}

static inline int ntdll_wcsncmp( const WCHAR *str1, const WCHAR *str2, int n )
{
    if (n <= 0) return 0;
    while ((--n > 0) && *str1 && (*str1 == *str2)) { str1++; str2++; }
    return *str1 - *str2;
}

static inline WCHAR *ntdll_wcschr( const WCHAR *str, WCHAR ch )
{
    do { if (*str == ch) return (WCHAR *)(ULONG_PTR)str; } while (*str++);
    return NULL;
}

#ifdef __i386_on_x86_64__
static inline WCHAR * HOSTPTR ntdll_wcschr( const WCHAR * HOSTPTR str, WCHAR ch ) __attribute__((overloadable))
{
    do { if (*str == ch) return (WCHAR * HOSTPTR)(ULONGLONG)str; } while (*str++);
    return NULL;
}
#endif

static inline WCHAR * HOSTPTR ntdll_wcsrchr( const WCHAR * HOSTPTR str, WCHAR ch )
{
    WCHAR * HOSTPTR ret = NULL;
    do { if (*str == ch) ret = (WCHAR * HOSTPTR)(ULONG_HOSTPTR)str; } while (*str++);
    return ret;
}

static inline WCHAR * HOSTPTR ntdll_wcspbrk( const WCHAR * HOSTPTR str, const WCHAR * HOSTPTR accept )
{
    for ( ; *str; str++) if (ntdll_wcschr( accept, *str )) return (WCHAR * HOSTPTR)(ULONG_HOSTPTR)str;
    return NULL;
}

static inline SIZE_T ntdll_wcsspn( const WCHAR * HOSTPTR str, const WCHAR * HOSTPTR accept )
{
    const WCHAR * HOSTPTR ptr;
    for (ptr = str; *ptr; ptr++) if (!ntdll_wcschr( accept, *ptr )) break;
    return ptr - str;
}

static inline SIZE_T ntdll_wcscspn( const WCHAR * HOSTPTR str, const WCHAR * HOSTPTR reject )
{
    const WCHAR * HOSTPTR ptr;
    for (ptr = str; *ptr; ptr++) if (ntdll_wcschr( reject, *ptr )) break;
    return ptr - str;
}

static inline int ntdll_wcsicmp( const WCHAR * HOSTPTR str1, const WCHAR * HOSTPTR str2 )
{
    int ret;
    for (;;)
    {
        if ((ret = ntdll_towupper( *str1 ) - ntdll_towupper( *str2 )) || !*str1) return ret;
        str1++;
        str2++;
    }
}

static inline int ntdll_wcsnicmp( const WCHAR * HOSTPTR str1, const WCHAR * HOSTPTR str2, int n )
{
    int ret;
    for (ret = 0; n > 0; n--, str1++, str2++)
        if ((ret = ntdll_towupper(*str1) - ntdll_towupper(*str2)) || !*str1) break;
    return ret;
}

#define wcslen(str)        ntdll_wcslen(str)
#define wcscpy(dst,src)    ntdll_wcscpy(dst,src)
#define wcscat(dst,src)    ntdll_wcscat(dst,src)
#define wcscmp(s1,s2)      ntdll_wcscmp(s1,s2)
#define wcsncmp(s1,s2,n)   ntdll_wcsncmp(s1,s2,n)
#define wcschr(str,ch)     ntdll_wcschr(str,ch)
#define wcsrchr(str,ch)    ntdll_wcsrchr(str,ch)
#define wcspbrk(str,ac)    ntdll_wcspbrk(str,ac)
#define wcsspn(str,ac)     ntdll_wcsspn(str,ac)
#define wcscspn(str,rej)   ntdll_wcscspn(str,rej)
#define wcsicmp(s1, s2)    ntdll_wcsicmp(s1,s2)
#define wcsnicmp(s1, s2,n) ntdll_wcsnicmp(s1,s2,n)
#define wcsupr(str)        ntdll_wcsupr(str)
#define towupper(c)        ntdll_towupper(c)
#define towlower(c)        ntdll_towlower(c)

#endif /* WINE_UNIX_LIB */

#include <wine/winheader_exit.h>

#endif  /* __WINE_WINE_UNIXLIB_H */
