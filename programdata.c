#define UNICODE
#include "programdata.h"
#include "array.h"
#include "dwordhashmap.h"

#include <windows.h>
#include <tlhelp32.h>
#include <wchar.h>

// Types.
typedef struct
{
    DWORD PID;
    DWORD ParentPID;
    DWORD Priority;
    WCHAR pExeFile [MAX_PATH];
} ProcessData;

typedef struct
{
    DWORD ID;
    DWORD Priority;
} ThreadData;

typedef struct
{
    WCHAR Path [MAX_PATH];
    DWORD Size;
} ModuleData;

ARRAY_H (ProcessData)
ARRAY_H (ThreadData)
ARRAY_H (ModuleData)

// Static vars.
ProcessDataArrayHandle hProcessDataArray = 0;
DWordHashMapHandle hThreadMap = 0;
DWordHashMapHandle hModuleMap = 0;

DWORD dCurrentProcessIterationIndex = 0;
DWORD dCurrentThreadIterationIndex = 0;
DWORD hCurrentThreadIterationHandle = 0;
DWORD dCurrentModuleIterationIndex = 0;
DWORD hCurrentModuleIterationHandle = 0;

// Implementations.
void DestructThreadDataEntry (DWordPair *pair)
{
    ThreadDataArray_Destruct (pair->dValue);
}

void DestructModuleDataEntry (DWordPair *pair)
{
    ModuleDataArray_Destruct (pair->dValue);
}

void ReloadProgramData ()
{
    if (hProcessDataArray)
    {
        ProcessDataArray_Destruct (hProcessDataArray);
    }

    if (hThreadMap)
    {
        DWordHashMap_DestructWithCallback (hThreadMap, DestructThreadDataEntry);
    }

    if (hModuleMap)
    {
        DWordHashMap_DestructWithCallback (hModuleMap, DestructModuleDataEntry);
    }

    HANDLE hProcessShapshot = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processEntry;

    HANDLE hThreadShapshot = CreateToolhelp32Snapshot (TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry;

    processEntry.dwSize = sizeof (PROCESSENTRY32);
    threadEntry.dwSize = sizeof (THREADENTRY32);

    hProcessDataArray = ProcessDataArray_Create (0);
    hThreadMap = DWordHashMap_Create (1347);
    hModuleMap = DWordHashMap_Create (1347);

    if (Process32First (hProcessShapshot, &processEntry))
    {
        do
        {
            ProcessData data;
            data.PID = processEntry.th32ProcessID;
            data.ParentPID = processEntry.th32ParentProcessID;
            data.Priority = processEntry.pcPriClassBase;
            wcsncpy (data.pExeFile, processEntry.szExeFile, MAX_PATH);
            ProcessDataArray_Push (hProcessDataArray, &data);
        }
        while (Process32Next (hProcessShapshot, &processEntry));
    }

    if (Thread32First (hThreadShapshot, &threadEntry))
    {
        do
        {
            DWORD *hArrayPtr = DWordHashMap_GetValue (hThreadMap, threadEntry.th32OwnerProcessID);
            DWORD hArray;
            ThreadData data;

            if (hArrayPtr == NULL)
            {
                DWORD hNewArray = ThreadDataArray_Create (1);
                DWordHashMap_InsertValue (hThreadMap, threadEntry.th32OwnerProcessID, hNewArray);
                hArray = hNewArray;
            }
            else
            {
                hArray = *hArrayPtr;
            }

            data.ID = threadEntry.th32ThreadID;
            data.Priority = threadEntry.tpBasePri;
            ThreadDataArray_Push (hArray, &data);
        }
        while (Thread32Next (hThreadShapshot, &threadEntry));
    }

    unsigned long uProcessCount = ProcessCount ();
    for (unsigned long uIndex = 0; uIndex < uProcessCount; ++uIndex)
    {
        HANDLE hModuleShapshot = CreateToolhelp32Snapshot (
                    TH32CS_SNAPMODULE, ProcessDataArray_Index (hProcessDataArray, uIndex)->PID);

        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof (MODULEENTRY32);

        if (Module32First (hModuleShapshot, &moduleEntry))
        {
            do
            {
                DWORD *hArrayPtr = DWordHashMap_GetValue (hModuleMap, moduleEntry.th32ProcessID);
                DWORD hArray;
                ModuleData data;

                if (hArrayPtr == NULL)
                {
                    DWORD hNewArray = ModuleDataArray_Create (1);
                    DWordHashMap_InsertValue (hModuleMap, moduleEntry.th32ProcessID, hNewArray);
                    hArray = hNewArray;
                }
                else
                {
                    hArray = *hArrayPtr;
                }

                data.Size = moduleEntry.modBaseSize;
                wcscpy (data.Path, moduleEntry.szExePath);
                ModuleDataArray_Push (hArray, &data);
            }
            while (Module32Next (hModuleShapshot, &moduleEntry));
            CloseHandle (hModuleShapshot);
        }
    }

    CloseHandle (hProcessShapshot);
    CloseHandle (hThreadShapshot);
}

DWORD ProcessCount ()
{
    return ProcessDataArray_Size (hProcessDataArray);
}

DWORD ThreadCount (DWORD dProcess)
{
    DWORD *hThreadDataArrayPtr = DWordHashMap_GetValue (hThreadMap, dProcess);
    if (hThreadDataArrayPtr != NULL)
    {
        return ThreadDataArray_Size (*hThreadDataArrayPtr);
    }
    else
    {
        return 0;
    }
}

DWORD ModuleCount (DWORD dProcess)
{
    DWORD *hModuleDataArrayPtr = DWordHashMap_GetValue (hModuleMap, dProcess);
    if (hModuleDataArrayPtr != NULL)
    {
        return ModuleDataArray_Size (*hModuleDataArrayPtr);
    }
    else
    {
        return 0;
    }
}

BOOL GetProcessData (DWORD dProcess, DWORD *pParentPID, DWORD *pPriority, WCHAR *pExeFile)
{
    unsigned long uProcessCount = ProcessCount ();
    for (unsigned long uIndex = 0; uIndex < uProcessCount; ++uIndex)
    {
        ProcessData *data = ProcessDataArray_Index (hProcessDataArray, uIndex);
        if (data->PID == dProcess)
        {
            *pParentPID = data->ParentPID;
            *pPriority = data->Priority;
            wcsncpy (pExeFile, data->pExeFile, MAX_PATH);
            return TRUE;
        }
    }

    return FALSE;
}

void ProcessIteration_Start  ()
{
    dCurrentProcessIterationIndex = 0;
}

BOOL ProcessIteration_Next ()
{
    ++dCurrentProcessIterationIndex;
    return dCurrentProcessIterationIndex < ProcessDataArray_Size (hProcessDataArray);
}

DWORD ProcessIteration_PID ()
{
    return ProcessDataArray_Index (hProcessDataArray, dCurrentProcessIterationIndex)->PID;
}

DWORD ProcessIteration_ParentPID ()
{
    return ProcessDataArray_Index (hProcessDataArray, dCurrentProcessIterationIndex)->ParentPID;
}

DWORD ProcessIteration_Priority ()
{
    return ProcessDataArray_Index (hProcessDataArray, dCurrentProcessIterationIndex)->Priority;
}

WCHAR *ProcessIteration_ExeFile ()
{
    return ProcessDataArray_Index (hProcessDataArray, dCurrentProcessIterationIndex)->pExeFile;
}

BOOL ThreadIteration_Start (DWORD dProcess)
{
    DWORD *handlePtr = DWordHashMap_GetValue (hThreadMap, dProcess);
    if (handlePtr == NULL)
    {
        return FALSE;
    }

    hCurrentThreadIterationHandle = *handlePtr;
    dCurrentThreadIterationIndex = 0;
    return dCurrentThreadIterationIndex < ThreadDataArray_Size (hCurrentThreadIterationHandle);
}

BOOL ThreadIteration_Next ()
{
    ++dCurrentThreadIterationIndex;
    return dCurrentThreadIterationIndex < ThreadDataArray_Size (hCurrentThreadIterationHandle);
}

DWORD ThreadIteration_ID ()
{
    return ThreadDataArray_Index (hCurrentThreadIterationHandle, dCurrentThreadIterationIndex)->ID;
}

DWORD ThreadIteration_Priority ()
{
    return ThreadDataArray_Index (hCurrentThreadIterationHandle, dCurrentThreadIterationIndex)->Priority;
}

BOOL ModuleIteration_Start (DWORD dProcess)
{
    DWORD *handlePtr = DWordHashMap_GetValue (hModuleMap, dProcess);
    if (handlePtr == NULL)
    {
        return FALSE;
    }

    hCurrentModuleIterationHandle = *handlePtr;
    dCurrentModuleIterationIndex = 0;
    return dCurrentModuleIterationIndex < ModuleDataArray_Size (hCurrentModuleIterationHandle);
}

BOOL ModuleIteration_Next ()
{
    ++dCurrentModuleIterationIndex;
    return dCurrentModuleIterationIndex < ModuleDataArray_Size (hCurrentModuleIterationHandle);
}

WCHAR *ModuleIteration_Path ()
{
    return ModuleDataArray_Index (hCurrentModuleIterationHandle, dCurrentModuleIterationIndex)->Path;
}

DWORD ModuleIteration_Size ()
{
    return ModuleDataArray_Index (hCurrentModuleIterationHandle, dCurrentModuleIterationIndex)->Size;
}

ARRAY_C (ProcessData)
ARRAY_C (ThreadData)
ARRAY_C (ModuleData)
