#define UNICODE
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0900
#define ERROR_EXIT 1

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <wchar.h>

#include "programdata.h"
#include "dwordhashmap.h"
#include "mainwindow.h"
#include "processinfowindow.h"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icex;
    MSG message;

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx (&icex);

    FreeConsole ();
    ReloadProgramData ();
    if (!SetupMainWindow (hInstance, nCmdShow) || !InitProcessInfoDependencies (hInstance, nCmdShow))
    {
        return ERROR_EXIT;
    }

    while (GetMessage (&message, NULL, 0, 0) > 0)
    {
        TranslateMessage (&message);
        DispatchMessage (&message);
    }

    return message.wParam;
}
