/*
 * Unit tests for registry functions
 *
 * Copyright (c) 2002 Alexandre Julliard
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <assert.h>
#include "wine/test.h"
#include "winbase.h"
#include "winreg.h"
#include "winerror.h"

static HKEY hkey_main;

/* delete key and all its subkeys */
static DWORD delete_key( HKEY hkey )
{
    WCHAR name[MAX_PATH];
    DWORD ret;

    while (!(ret = RegEnumKeyW(hkey, 0, name, sizeof(name))))
    {
        HKEY tmp;
        if (!(ret = RegOpenKeyExW( hkey, name, 0, KEY_ENUMERATE_SUB_KEYS, &tmp )))
        {
            ret = delete_key( tmp );
            RegCloseKey( tmp );
        }
        if (ret) break;
    }
    if (ret != ERROR_NO_MORE_ITEMS) return ret;
    RegDeleteKeyA( hkey, NULL );
    return 0;
}

static void setup_main_key(void)
{
    if (RegOpenKeyA( HKEY_CURRENT_USER, "Software\\Wine\\Test", &hkey_main )) delete_key( hkey_main );

    assert (!RegCreateKeyExA( HKEY_CURRENT_USER, "Software\\Wine\\Test", 0, NULL,
                              REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey_main, NULL ));
}

static void test_enum_value(void)
{
    DWORD res;
    char value[20], data[20];
    WCHAR valueW[20], dataW[20];
    DWORD val_count, data_count, type;
    static const WCHAR foobarW[] = {'f','o','o','b','a','r',0};
    static const WCHAR testW[] = {'T','e','s','t',0};
    static const WCHAR xxxW[] = {'x','x','x','x','x','x','x','x',0};

    res = RegSetValueExA( hkey_main, "Test", 0, REG_SZ, (BYTE *)"foobar", 7 );
    ok( res == 0, "RegSetValueExA failed error %ld", res );

    /* overflow both name and data */
    val_count = 2;
    data_count = 2;
    type = 1234;
    strcpy( value, "xxxxxxxxxx" );
    strcpy( data, "xxxxxxxxxx" );
    res = RegEnumValueA( hkey_main, 0, value, &val_count, NULL, &type, data, &data_count );
    ok( res == ERROR_MORE_DATA, "expected ERROR_MORE_DATA, got %ld", res );
    ok( val_count == 2, "val_count set to %ld", val_count );
    ok( data_count == 7, "data_count set to %ld instead of 7", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !strcmp( value, "xxxxxxxxxx" ), "value set to '%s'", value );
    ok( !strcmp( data, "xxxxxxxxxx" ), "data set to '%s'", data );

    /* overflow name */
    val_count = 3;
    data_count = 20;
    type = 1234;
    strcpy( value, "xxxxxxxxxx" );
    strcpy( data, "xxxxxxxxxx" );
    res = RegEnumValueA( hkey_main, 0, value, &val_count, NULL, &type, data, &data_count );
    ok( res == ERROR_MORE_DATA, "expected ERROR_MORE_DATA, got %ld", res );
    ok( val_count == 3, "val_count set to %ld", val_count );
    ok( data_count == 7, "data_count set to %ld instead of 7", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !strcmp( value, "Te" ), "value set to '%s' instead of 'Te'", value );
    ok( !strcmp( data, "foobar" ), "data set to '%s' instead of 'foobar'", data );

    /* overflow empty name */
    val_count = 0;
    data_count = 20;
    type = 1234;
    strcpy( value, "xxxxxxxxxx" );
    strcpy( data, "xxxxxxxxxx" );
    res = RegEnumValueA( hkey_main, 0, value, &val_count, NULL, &type, data, &data_count );
    ok( res == ERROR_MORE_DATA, "expected ERROR_MORE_DATA, got %ld", res );
    ok( val_count == 0, "val_count set to %ld", val_count );
    ok( data_count == 7, "data_count set to %ld instead of 7", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !strcmp( value, "xxxxxxxxxx" ), "value set to '%s'", value );
    ok( !strcmp( data, "foobar" ), "data set to '%s' instead of 'foobar'", data );

    /* overflow data */
    val_count = 20;
    data_count = 2;
    type = 1234;
    strcpy( value, "xxxxxxxxxx" );
    strcpy( data, "xxxxxxxxxx" );
    res = RegEnumValueA( hkey_main, 0, value, &val_count, NULL, &type, data, &data_count );
    ok( res == ERROR_MORE_DATA, "expected ERROR_MORE_DATA, got %ld", res );
    ok( val_count == 20, "val_count set to %ld", val_count );
    ok( data_count == 7, "data_count set to %ld instead of 7", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !strcmp( value, "xxxxxxxxxx" ), "value set to '%s'", value );
    ok( !strcmp( data, "xxxxxxxxxx" ), "data set to '%s'", data );

    /* no overflow */
    val_count = 20;
    data_count = 20;
    type = 1234;
    strcpy( value, "xxxxxxxxxx" );
    strcpy( data, "xxxxxxxxxx" );
    res = RegEnumValueA( hkey_main, 0, value, &val_count, NULL, &type, data, &data_count );
    ok( res == ERROR_SUCCESS, "expected ERROR_SUCCESS, got %ld", res );
    ok( val_count == 4, "val_count set to %ld instead of 4", val_count );
    ok( data_count == 7, "data_count set to %ld instead of 7", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !strcmp( value, "Test" ), "value is '%s' instead of Test", value );
    ok( !strcmp( data, "foobar" ), "data is '%s' instead of foobar", data );

    /* Unicode tests */

    res = RegSetValueExW( hkey_main, testW, 0, REG_SZ, (BYTE *)foobarW, 7*sizeof(WCHAR) );
    ok( res == 0, "RegSetValueExW failed error %ld", res );

    /* overflow both name and data */
    val_count = 2;
    data_count = 2;
    type = 1234;
    memcpy( valueW, xxxW, sizeof(xxxW) );
    memcpy( dataW, xxxW, sizeof(xxxW) );
    res = RegEnumValueW( hkey_main, 0, valueW, &val_count, NULL, &type, (BYTE*)dataW, &data_count );
    ok( res == ERROR_MORE_DATA, "expected ERROR_MORE_DATA, got %ld", res );
    ok( val_count == 2, "val_count set to %ld", val_count );
    ok( data_count == 7*sizeof(WCHAR), "data_count set to %ld instead of 7*sizeof(WCHAR)", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !memcmp( valueW, xxxW, sizeof(xxxW) ), "value modified" );
    ok( !memcmp( dataW, xxxW, sizeof(xxxW) ), "data modified" );

    /* overflow name */
    val_count = 3;
    data_count = 20;
    type = 1234;
    memcpy( valueW, xxxW, sizeof(xxxW) );
    memcpy( dataW, xxxW, sizeof(xxxW) );
    res = RegEnumValueW( hkey_main, 0, valueW, &val_count, NULL, &type, (BYTE*)dataW, &data_count );
    ok( res == ERROR_MORE_DATA, "expected ERROR_MORE_DATA, got %ld", res );
    ok( val_count == 3, "val_count set to %ld", val_count );
    ok( data_count == 7*sizeof(WCHAR), "data_count set to %ld instead of 7*sizeof(WCHAR)", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !memcmp( valueW, xxxW, sizeof(xxxW) ), "value modified" );
    ok( !memcmp( dataW, xxxW, sizeof(xxxW) ), "data modified" );

    /* overflow data */
    val_count = 20;
    data_count = 2;
    type = 1234;
    memcpy( valueW, xxxW, sizeof(xxxW) );
    memcpy( dataW, xxxW, sizeof(xxxW) );
    res = RegEnumValueW( hkey_main, 0, valueW, &val_count, NULL, &type, (BYTE*)dataW, &data_count );
    ok( res == ERROR_MORE_DATA, "expected ERROR_MORE_DATA, got %ld", res );
    ok( val_count == 4, "val_count set to %ld instead of 4", val_count );
    ok( data_count == 7*sizeof(WCHAR), "data_count set to %ld instead of 7*sizeof(WCHAR)", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !memcmp( valueW, testW, sizeof(testW) ), "value is not 'Test'" );
    ok( !memcmp( dataW, xxxW, sizeof(xxxW) ), "data modified" );

    /* no overflow */
    val_count = 20;
    data_count = 20;
    type = 1234;
    memcpy( valueW, xxxW, sizeof(xxxW) );
    memcpy( dataW, xxxW, sizeof(xxxW) );
    res = RegEnumValueW( hkey_main, 0, valueW, &val_count, NULL, &type, (BYTE*)dataW, &data_count );
    ok( res == ERROR_SUCCESS, "expected ERROR_SUCCESS, got %ld", res );
    ok( val_count == 4, "val_count set to %ld instead of 4", val_count );
    ok( data_count == 7*sizeof(WCHAR), "data_count set to %ld instead of 7*sizeof(WCHAR)", data_count );
    ok( type == REG_SZ, "type %ld is not REG_SZ", type );
    ok( !memcmp( valueW, testW, sizeof(testW) ), "value is not 'Test'" );
    ok( !memcmp( dataW, foobarW, sizeof(foobarW) ), "data is not 'foobar'" );

    /* cleanup */
    RegDeleteValueA( hkey_main, "Test" );
}

START_TEST(registry)
{
    setup_main_key();
    test_enum_value();

    /* cleanup */
    delete_key( hkey_main );
}
