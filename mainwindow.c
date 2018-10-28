#define UNICODE
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0900

#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#include "mainwindow.h"
#include "programdata.h"
#include "dwordhashmap.h"
#include "processinfowindow.h"

#define BUTTONS_IN_A_ROW 10
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700
#define BORDER_X 10
#define BORDER_Y 10
#define AVAILABLE_WIDTH (WINDOW_WIDTH - 2 * BORDER_X)
#define AVAILABLE_HEIGHT (WINDOW_HEIGHT - 2 * BORDER_Y)
#define DEFAULT_BUTTON_HEIGHT 25
#define SPACER 5

const WCHAR pMainWindowClassName [] = TEXT ("MainWindowClass");
HINSTANCE hInstance;
HWND hMainWindow;
HWND hUpdateButton;
DWordHashMapHandle hButtonMapping = 0;

void RecreateProcessButtons ();
LRESULT CALLBACK MainWindowProcedure (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_COMMAND:
        if (wParam == BN_CLICKED)
        {
            if (lParam == hUpdateButton)
            {
                ReloadProgramData ();
                RecreateProcessButtons ();
            }
            else
            {
                DWORD *pProcessID = DWordHashMap_GetValue (hButtonMapping, lParam);
                if (pProcessID != NULL)
                {
                    ShowProcessInfo (*pProcessID);
                }
            }
        }
        break;

    case WM_CLOSE:
        DestroyWindow (hWindow);
        break;

    case WM_DESTROY:
        PostQuitMessage (0);
        break;

    default:
        return DefWindowProc (hWindow, msg, wParam, lParam);
    }
    return 0;
}

void DestructMappingEntry (DWordPair *pair)
{
    DestroyWindow ((HWND) pair->dKey);
}

void RecreateProcessButtons ()
{
    if (hButtonMapping)
    {
        DWordHashMap_DestructWithCallback (hButtonMapping, DestructMappingEntry);
    }

    hButtonMapping = DWordHashMap_Create (1347);
    ProcessIteration_Start ();
    unsigned long uIndex = 0;

    do
    {
        WCHAR pBuffer [30];
        unsigned long uX = (AVAILABLE_WIDTH / BUTTONS_IN_A_ROW) * (uIndex % BUTTONS_IN_A_ROW);
        unsigned long uY = (DEFAULT_BUTTON_HEIGHT + SPACER) * (uIndex / BUTTONS_IN_A_ROW + 1);

        wsprintf (pBuffer, TEXT ("PID: %u."), ProcessIteration_PID ());
        HWND hButton = CreateWindow (TEXT ("BUTTON"), pBuffer,
                                     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                     BORDER_X + uX, BORDER_Y + uY,
                                     AVAILABLE_WIDTH / BUTTONS_IN_A_ROW - SPACER, DEFAULT_BUTTON_HEIGHT,
                                     hMainWindow, NULL, hInstance, NULL);

        DWordHashMap_InsertValue (hButtonMapping, hButton, ProcessIteration_PID ());
        ++uIndex;
    }
    while (ProcessIteration_Next ());
}

BOOL SetupMainWindow (HINSTANCE hMainInstance, int nCmdShow)
{
    WNDCLASSEX windowClass;
    hInstance = hMainInstance;

    windowClass.cbSize = sizeof (WNDCLASSEX);
    windowClass.style = 0;
    windowClass.lpfnWndProc = MainWindowProcedure;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor (NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = pMainWindowClassName;
    windowClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);

    if (!RegisterClassEx (&windowClass))
    {
        MessageBox (NULL, TEXT ("Window Registration Failed!"), TEXT ("Error!"),
                    MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    hMainWindow = CreateWindowEx (WS_EX_CLIENTEDGE, pMainWindowClassName, TEXT ("Process List Viewer"),
                                  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    hUpdateButton = CreateWindow (TEXT ("BUTTON"), TEXT ("Update list"),
                                  WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                  BORDER_X, BORDER_Y, AVAILABLE_WIDTH, DEFAULT_BUTTON_HEIGHT,
                                  hMainWindow, NULL, hInstance, NULL);

    if (hMainWindow == NULL || hUpdateButton == NULL)
    {
        MessageBox(NULL, TEXT ("Main window Creation Failed!"), TEXT ("Error!"),
                   MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    ShowWindow (hMainWindow, nCmdShow);
    UpdateWindow (hMainWindow);
    RecreateProcessButtons ();
    return TRUE;
}
