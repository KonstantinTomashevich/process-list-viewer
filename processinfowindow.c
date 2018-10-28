#define UNICODE
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0900

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700
#define BORDER_Y 10

#define THREAD_LABEL_X 10
#define THREAD_IN_A_ROW 2
#define THREAD_LABEL_WIDTH 150
#define THREAD_LABEL_HEIGHT 25
#define THREAD_LABEL_SPACER 0

#define MODULE_LABEL_X 320
#define MODULE_LABEL_WIDTH 660
#define MODULE_LABEL_HEIGHT 25
#define MODULE_LABEL_SPACER 0

#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#include "array.h"
#include "programdata.h"
#include "dwordhashmap.h"
#include "processinfowindow.h"
#include "programdata.h"

ARRAY_H (HWND)
const WCHAR pProcessInfoWindowClassName [] = TEXT ("ProcessInfoWindowClass");
HINSTANCE hInstance;
int nCmdShow;
WNDCLASSEX windowClassProcessInfo;

LRESULT CALLBACK ProcessInfoWindowProcedure (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CLOSE:
        DestroyWindow (hWindow);
        break;

    case WM_DESTROY:
        break;

    default:
        return DefWindowProc (hWindow, msg, wParam, lParam);
    }
    return 0;
}

BOOL InitProcessInfoDependencies (HINSTANCE hMainInstance, int nMainCmdShow)
{
    hInstance = hMainInstance;
    nCmdShow = nMainCmdShow;

    windowClassProcessInfo.cbSize = sizeof (WNDCLASSEX);
    windowClassProcessInfo.style = 0;
    windowClassProcessInfo.lpfnWndProc = ProcessInfoWindowProcedure;
    windowClassProcessInfo.cbClsExtra = 0;
    windowClassProcessInfo.cbWndExtra = 0;
    windowClassProcessInfo.hInstance = hInstance;
    windowClassProcessInfo.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    windowClassProcessInfo.hCursor = LoadCursor (NULL, IDC_ARROW);
    windowClassProcessInfo.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    windowClassProcessInfo.lpszMenuName = NULL;
    windowClassProcessInfo.lpszClassName = pProcessInfoWindowClassName;
    windowClassProcessInfo.hIconSm = LoadIcon (NULL, IDI_APPLICATION);

    if (!RegisterClassEx (&windowClassProcessInfo))
    {
        MessageBox (NULL, TEXT ("Proces Info window Registration Failed!"), TEXT ("Error!"),
                    MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    return TRUE;
}

void ShowProcessInfo (DWORD dPID)
{
    HWND hWindow;
    DWORD dParentPID;
    DWORD dPriority;
    WCHAR pBuffer [1024];
    WCHAR pMainExePath [MAX_PATH];

    GetProcessData (dPID, &dParentPID, &dPriority, pMainExePath);
    wsprintf (pBuffer, TEXT ("PID %u Parent PID %u Priority %u %Exe %s"),
              dPID, dParentPID, dPriority, pMainExePath);

    hWindow = CreateWindowEx (WS_EX_CLIENTEDGE, pProcessInfoWindowClassName, pBuffer,
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    wsprintf (pBuffer, TEXT ("Thread count: %u."), ThreadCount (dPID));
    CreateWindow (TEXT ("STATIC"), pBuffer,
                  WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
                  THREAD_LABEL_X, BORDER_Y, THREAD_LABEL_WIDTH * 2, THREAD_LABEL_HEIGHT,
                  hWindow, NULL, hInstance, NULL);

    if (ThreadIteration_Start (dPID))
    {
        unsigned long uIndex = 0;
        do
        {
            wsprintf (pBuffer, TEXT ("ID: %u. Priority: %u."),
                      ThreadIteration_ID (), ThreadIteration_Priority ());

            CreateWindow (TEXT ("STATIC"), pBuffer,
                          WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
                          THREAD_LABEL_X + THREAD_LABEL_WIDTH * (uIndex % 2),
                          BORDER_Y + (THREAD_LABEL_HEIGHT + THREAD_LABEL_SPACER) * (uIndex / 2 + 1),
                          THREAD_LABEL_WIDTH, THREAD_LABEL_HEIGHT,
                          hWindow, NULL, hInstance, NULL);
            ++uIndex;
        }
        while (ThreadIteration_Next ());
    }

    wsprintf (pBuffer, TEXT ("Module count: %u."), ModuleCount (dPID));
    CreateWindow (TEXT ("STATIC"), pBuffer,
                  WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
                  MODULE_LABEL_X, BORDER_Y, MODULE_LABEL_WIDTH, MODULE_LABEL_HEIGHT,
                  hWindow, NULL, hInstance, NULL);

    if (ModuleIteration_Start (dPID))
    {
        unsigned long uIndex = 0;
        do
        {
            wsprintf (pBuffer, TEXT ("Size: %u. Path: %s."),
                      ModuleIteration_Size (), ModuleIteration_Path ());

            CreateWindow (TEXT ("STATIC"), pBuffer,
                          WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFTNOWORDWRAP,
                          MODULE_LABEL_X, BORDER_Y + (MODULE_LABEL_HEIGHT + MODULE_LABEL_SPACER) * (uIndex + 1),
                          MODULE_LABEL_WIDTH, MODULE_LABEL_HEIGHT,
                          hWindow, NULL, hInstance, NULL);
            ++uIndex;
        }
        while (ModuleIteration_Next ());
    }

    ShowWindow (hWindow, nCmdShow);
    UpdateWindow (hWindow);
}

ARRAY_C (HWND)
