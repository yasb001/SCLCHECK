/*
 *  @file  : ScdIcdCheckService_ServiceMain.cpp
 *  @author: luteng
 *  @date  : 2015-07-01 16:50:06.835
 *  @note  : Generated by SlxTemplates
 */

#include "ScdIcdCheckService_ServiceMain.h"
#include "ScdIcdCheckService_ServiceTools.h"
#include "ScdIcdCheckService_ServiceWork.h"
#include <winsvc.h>

static SERVICE_STATUS_HANDLE g_hStatus = NULL;
static HANDLE g_hServiceMainThread = NULL;
static HANDLE g_hStopEvent = NULL;

BOOL ReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode)
{
    static SERVICE_STATUS status = {SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS};

    if(dwCurrentState == SERVICE_START_PENDING)
    {
        status.dwControlsAccepted = 0;
    }
    else
    {
        status.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
    }

    status.dwCurrentState = dwCurrentState;
    status.dwWin32ExitCode = dwWin32ExitCode;

    return SetServiceStatus(g_hStatus, &status);
}

DWORD WINAPI ServiceHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
    if(dwControl == SERVICE_CONTROL_STOP)
    {
        DWORD dwExitCode = 0;

        ReportStatus(SERVICE_STOP_PENDING, ERROR_SUCCESS);
        SetEvent(g_hStopEvent);

        if (WAIT_OBJECT_0 != WaitForSingleObject(g_hServiceMainThread, 4043))
        {
            TerminateThread(g_hServiceMainThread, 0);
            Sleep(525);
        }

        CloseHandle(g_hServiceMainThread);
        g_hServiceMainThread = NULL;

        ReportStatus(SERVICE_STOPPED, ERROR_SUCCESS);
    }
    else if(dwControl == SERVICE_CONTROL_SHUTDOWN)
    {

    }
    else if(dwControl == SERVICE_CONTROL_INTERROGATE)
    {
        if (WAIT_OBJECT_0 == WaitForSingleObject(g_hServiceMainThread, 0))
        {
            ReportStatus(SERVICE_STOPPED, ERROR_SUCCESS);
        }
        else
        {
            ReportStatus(SERVICE_RUNNING, ERROR_SUCCESS);
        }
    }

    return ERROR_SUCCESS;
}

DWORD CALLBACK MonitorProc(LPVOID lpParam)
{
    HANDLE hObjects[] = {g_hServiceMainThread, g_hStopEvent};

    if (WaitForMultipleObjects(RTL_NUMBER_OF(hObjects), hObjects, FALSE, INFINITE) == WAIT_OBJECT_0)
    {
        ServiceHandlerEx(SERVICE_CONTROL_STOP, 0, NULL, NULL);
    }

    return 0;
}

VOID WINAPI ScdIcdCheckService_ServiceMainProc(DWORD dwArgc, LPTSTR *lpszArgv)
{
    g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_hStatus = RegisterServiceCtrlHandlerEx(_SERVER_SERVICE_NAME, ServiceHandlerEx, 0);

    ReportStatus(SERVICE_START_PENDING, ERROR_SUCCESS);
    g_hServiceMainThread = CreateThread(NULL, 0, ScdIcdCheckServiceWorkProc, g_hStopEvent, 0, NULL);
    CloseHandle(CreateThread(NULL, 0, MonitorProc, NULL, 0, NULL));
    ReportStatus(SERVICE_RUNNING, ERROR_SUCCESS);
}
