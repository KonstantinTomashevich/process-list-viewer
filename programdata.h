#pragma once
#include <minwindef.h>

void ReloadProgramData ();
DWORD ProcessCount ();
DWORD ThreadCount (DWORD dProcess);
DWORD ModuleCount (DWORD dProcess);
BOOL GetProcessData (DWORD dProcess, DWORD *pParentPID, DWORD *pPriority, WCHAR *pExeFile);

void ProcessIteration_Start  ();
BOOL ProcessIteration_Next ();
DWORD ProcessIteration_PID ();
DWORD ProcessIteration_ParentPID ();
DWORD ProcessIteration_Priority ();
WCHAR *ProcessIteration_ExeFile ();

BOOL ThreadIteration_Start(DWORD dProcess);
BOOL ThreadIteration_Next ();
DWORD ThreadIteration_ID ();
DWORD ThreadIteration_Priority ();

BOOL ModuleIteration_Start (DWORD dProcess);
BOOL ModuleIteration_Next ();
WCHAR *ModuleIteration_Path ();
DWORD ModuleIteration_Size ();
