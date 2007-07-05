/*
 * Unit tests for service functions
 *
 * Copyright (c) 2007 Paul Vriens
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

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "winerror.h"
#include "winsvc.h"

#include "wine/test.h"

static void test_open_scm(void)
{
    SC_HANDLE scm_handle;

    /* No access rights */
    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA(NULL, NULL, 0);
    ok(scm_handle != NULL, "Expected success\n");
    ok(GetLastError() == ERROR_SUCCESS    /* W2K3, Vista */ ||
       GetLastError() == 0xdeadbeef       /* NT4, XP */ ||
       GetLastError() == ERROR_IO_PENDING /* W2K */,
       "Expected ERROR_SUCCESS, ERROR_IO_PENDING or 0xdeadbeef, got %d\n", GetLastError());
    CloseServiceHandle(scm_handle);

    /* Unknown database name */
    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA(NULL, "DoesNotExist", SC_MANAGER_CONNECT);
    ok(!scm_handle, "Expected failure\n");
    ok(GetLastError() == ERROR_INVALID_NAME, "Expected ERROR_INVALID_NAME, got %d\n", GetLastError());
    CloseServiceHandle(scm_handle); /* Just in case */

    /* MSDN says only ServiceActive is allowed, or NULL */
    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA(NULL, SERVICES_FAILED_DATABASEA, SC_MANAGER_CONNECT);
    ok(!scm_handle, "Expected failure\n");
    ok(GetLastError() == ERROR_DATABASE_DOES_NOT_EXIST, "Expected ERROR_DATABASE_DOES_NOT_EXIST, got %d\n", GetLastError());
    CloseServiceHandle(scm_handle); /* Just in case */

    /* Remote unknown host */
    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA("DOESNOTEXIST", SERVICES_ACTIVE_DATABASEA, SC_MANAGER_CONNECT);
    ok(!scm_handle, "Expected failure\n");
    todo_wine
    ok(GetLastError() == RPC_S_SERVER_UNAVAILABLE, "Expected RPC_S_SERVER_UNAVAILABLE, got %d\n", GetLastError());
    CloseServiceHandle(scm_handle); /* Just in case */

    /* Proper call with an empty hostname */
    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA("", SERVICES_ACTIVE_DATABASEA, SC_MANAGER_CONNECT);
    ok(scm_handle != NULL, "Expected success\n");
    ok(GetLastError() == ERROR_SUCCESS          /* W2K3, Vista */ ||
       GetLastError() == ERROR_ENVVAR_NOT_FOUND /* NT4 */ ||
       GetLastError() == 0xdeadbeef             /* XP */ ||
       GetLastError() == ERROR_IO_PENDING       /* W2K */,
       "Expected ERROR_SUCCESS, ERROR_IO_PENDING, ERROR_ENVVAR_NOT_FOUND or 0xdeadbeef, got %d\n", GetLastError());
    CloseServiceHandle(scm_handle);

    /* Again a correct one */
    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT);
    ok(scm_handle != NULL, "Expected success\n");
    ok(GetLastError() == ERROR_SUCCESS    /* W2K3, Vista */ ||
       GetLastError() == 0xdeadbeef       /* NT4, XP */ ||
       GetLastError() == ERROR_IO_PENDING /* W2K */,
       "Expected ERROR_SUCCESS, ERROR_IO_PENDING or 0xdeadbeef, got %d\n", GetLastError());
    CloseServiceHandle(scm_handle);
}

static void test_open_svc(void)
{
    SC_HANDLE scm_handle, svc_handle;

    /* All NULL (invalid access rights) */
    SetLastError(0xdeadbeef);
    svc_handle = OpenServiceA(NULL, NULL, 0);
    ok(!svc_handle, "Expected failure\n");
    todo_wine
    ok(GetLastError() == ERROR_INVALID_HANDLE, "Expected ERROR_INVALID_HANDLE, got %d\n", GetLastError());

    /* TODO: Add some tests with invalid handles. These produce errors on Windows but crash on Wine */

    /* NULL service */
    scm_handle = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT);
    SetLastError(0xdeadbeef);
    svc_handle = OpenServiceA(scm_handle, NULL, GENERIC_READ);
    ok(!svc_handle, "Expected failure\n");
    ok(GetLastError() == ERROR_INVALID_ADDRESS   /* W2K, XP, W2K3, Vista */ ||
       GetLastError() == ERROR_INVALID_PARAMETER /* NT4 */,
       "Expected ERROR_INVALID_ADDRESS or ERROR_INVALID_PARAMETER, got %d\n", GetLastError());

    /* Non-existent service */
    scm_handle = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT);
    SetLastError(0xdeadbeef);
    svc_handle = OpenServiceA(scm_handle, "deadbeef", GENERIC_READ);
    ok(!svc_handle, "Expected failure\n");
    ok(GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST, "Expected ERROR_SERVICE_DOES_NOT_EXIST, got %d\n", GetLastError());
    CloseServiceHandle(scm_handle);

    /* Proper SCM handle but different access rights */
    scm_handle = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT);
    SetLastError(0xdeadbeef);
    svc_handle = OpenServiceA(scm_handle, "Spooler", GENERIC_WRITE);
    if (!svc_handle && (GetLastError() == ERROR_ACCESS_DENIED))
        skip("Not enough rights to get a handle to the service\n");
    else
    {
        ok(svc_handle != NULL, "Expected success\n");
        ok(GetLastError() == ERROR_SUCCESS    /* W2K3, Vista */ ||
           GetLastError() == ERROR_IO_PENDING /* W2K */ ||
           GetLastError() == 0xdeadbeef       /* XP, NT4 */,
           "Expected ERROR_SUCCESS or 0xdeadbeef, got %d\n", GetLastError());
        CloseServiceHandle(svc_handle);
    }
    CloseServiceHandle(scm_handle);
}

static void test_sequence(void)
{
    SC_HANDLE scm_handle, svc_handle;
    BOOL ret;
    QUERY_SERVICE_CONFIGA *config;
    DWORD given, needed;
    static const CHAR servicename [] = "Winetest";
    static const CHAR displayname [] = "Winetest dummy service";
    static const CHAR pathname    [] = "we_dont_care.exe";
    static const CHAR dependencies[] = "Master1\0Master2\0+MasterGroup1\0\0";
    static const CHAR password    [] = "";
    static const CHAR empty       [] = "";
    static const CHAR localsystem [] = "LocalSystem";

    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA(NULL, NULL, GENERIC_ALL);

    if (!scm_handle && (GetLastError() == ERROR_ACCESS_DENIED))
    {
        skip("Not enough rights to get a handle to the manager\n");
        return;
    }
    else
        ok(scm_handle != NULL, "Could not get a handle to the manager: %d\n", GetLastError());

    if (!scm_handle) return;

    /* Create a dummy service */
    SetLastError(0xdeadbeef);
    svc_handle = CreateServiceA(scm_handle, servicename, displayname, GENERIC_ALL,
        SERVICE_INTERACTIVE_PROCESS | SERVICE_WIN32_OWN_PROCESS, SERVICE_DISABLED, SERVICE_ERROR_IGNORE,
        pathname, NULL, NULL, dependencies, NULL, password);

    if (!svc_handle && (GetLastError() == ERROR_SERVICE_EXISTS))
    {
        /* We try and open the service and do the rest of the tests. Some could
         * fail if the tests were changed between these runs.
         */
        trace("Deletion probably didn't work last time\n");
        SetLastError(0xdeadbeef);
        svc_handle = OpenServiceA(scm_handle, servicename, GENERIC_ALL);
        if (!svc_handle && (GetLastError() == ERROR_ACCESS_DENIED))
        {
            skip("Not enough rights to open the service\n");
            CloseServiceHandle(scm_handle);        
            return;
        }
        ok(svc_handle != NULL, "Could not open the service : %d\n", GetLastError());
    }
    else if (!svc_handle && (GetLastError() == ERROR_ACCESS_DENIED))
    {
        skip("Not enough rights to create the service\n");
        CloseServiceHandle(scm_handle);        
        return;
    }
    else
        ok(svc_handle != NULL, "Could not create the service : %d\n", GetLastError());

    if (!svc_handle) return;

    /* TODO:
     * Before we do a QueryServiceConfig we should check the registry. This will make sure
     * that the correct keys are used.
     */

    /* Request the size for the buffer */
    SetLastError(0xdeadbeef);
    ret = QueryServiceConfigA(svc_handle, NULL, 0, &needed);
    ok(!ret, "Expected failure\n");
    ok(GetLastError() == ERROR_INSUFFICIENT_BUFFER, "Expected ERROR_INSUFFICIENT_BUFFER, got %d\n", GetLastError());

    config = HeapAlloc(GetProcessHeap(), 0, needed);
    given = needed;
    SetLastError(0xdeadbeef);
    ret = QueryServiceConfigA(svc_handle, config, given, &needed);
    ok(ret, "Expected success\n");
    todo_wine
    {
    ok(GetLastError() == ERROR_SUCCESS    /* W2K3 */||
       GetLastError() == 0xdeadbeef       /* NT4, XP, Vista */ ||
       GetLastError() == ERROR_IO_PENDING /* W2K */,
        "Expected ERROR_SUCCESS, ERROR_IO_PENDING or 0xdeadbeef, got %d\n", GetLastError());
    ok(given == needed, "Expected the given (%d) and needed (%d) buffersizes to be equal\n", given, needed);
    }
    ok(config->lpBinaryPathName && config->lpLoadOrderGroup && config->lpDependencies && config->lpServiceStartName &&
        config->lpDisplayName, "Expected all string struct members to be non-NULL\n");
    ok(config->dwServiceType == (SERVICE_INTERACTIVE_PROCESS | SERVICE_WIN32_OWN_PROCESS),
        "Expected SERVICE_INTERACTIVE_PROCESS | SERVICE_WIN32_OWN_PROCESS, got %d\n", config->dwServiceType);
    ok(config->dwStartType == SERVICE_DISABLED, "Expected SERVICE_DISABLED, got %d\n", config->dwStartType);
    ok(config->dwErrorControl == SERVICE_ERROR_IGNORE, "Expected SERVICE_ERROR_IGNORE, got %d\n", config->dwErrorControl);
    ok(!strcmp(config->lpBinaryPathName, pathname), "Expected '%s', got '%s'\n", pathname, config->lpBinaryPathName);
    ok(!strcmp(config->lpLoadOrderGroup, empty), "Expected an empty string, got '%s'\n", config->lpLoadOrderGroup);
    ok(config->dwTagId == 0, "Expected 0, got %d\n", config->dwTagId);
    /* TODO: Show the double 0 terminated string */
    todo_wine
    {
    ok(!memcmp(config->lpDependencies, dependencies, sizeof(dependencies)), "Wrong string\n");
    ok(!strcmp(config->lpServiceStartName, localsystem), "Expected 'LocalSystem', got '%s'\n", config->lpServiceStartName);
    }
    ok(!strcmp(config->lpDisplayName, displayname), "Expected '%s', got '%s'\n", displayname, config->lpDisplayName);
    
    SetLastError(0xdeadbeef);
    ret = DeleteService(svc_handle);
    ok(ret, "Expected success\n");
    ok(GetLastError() == ERROR_SUCCESS    /* W2K3 */||
       GetLastError() == 0xdeadbeef       /* NT4, XP, Vista */ ||
       GetLastError() == ERROR_IO_PENDING /* W2K */,
        "Expected ERROR_SUCCESS, ERROR_IO_PENDING or 0xdeadbeef, got %d\n", GetLastError());
    
    CloseServiceHandle(svc_handle);
    CloseServiceHandle(scm_handle);
}

START_TEST(service)
{
    SC_HANDLE scm_handle;

    /* Bail out if we are on win98 */
    SetLastError(0xdeadbeef);
    scm_handle = OpenSCManagerA(NULL, NULL, GENERIC_ALL);

    if (!scm_handle && (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED))
    {
        skip("OpenSCManagerA is not implemented, we are most likely on win9x\n");
        return;
    }
    CloseServiceHandle(scm_handle);

    /* First some parameter checking */
    test_open_scm();
    test_open_svc();
    /* Test the creation, querying and deletion of a service */
    test_sequence();
}
