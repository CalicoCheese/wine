/*
 * Copyright 2017 Nikolay Sivov
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

#include "wine/winheader_enter.h"

NTSTATUS WINAPI LsaEnumerateAccounts(LSA_HANDLE,PLSA_ENUMERATION_HANDLE,PVOID*,ULONG,PULONG);
NTSTATUS WINAPI LsaLookupPrivilegeDisplayName(LSA_HANDLE policy, LSA_UNICODE_STRING *name,
    LSA_UNICODE_STRING **display_name, SHORT *language);
NTSTATUS WINAPI LsaLookupPrivilegeName(LSA_HANDLE policy, LUID *value, LSA_UNICODE_STRING **name);

#include "wine/winheader_exit.h"