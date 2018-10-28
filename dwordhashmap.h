#pragma once
#include <minwindef.h>

typedef struct
{
    DWORD dKey;
    DWORD dValue;
} DWordPair;

typedef unsigned long DWordHashMapHandle;
typedef void (*DWordHashMap_DestructCallback) (DWordPair *pair);

DWordHashMapHandle DWordHashMap_Create (unsigned long uBuckets);
void DWordHashMap_InsertValue (DWordHashMapHandle handle, DWORD dKey, DWORD dValue);
DWORD *DWordHashMap_GetValue (DWordHashMapHandle handle, DWORD dKey);
void DWordHashMap_Destruct (DWordHashMapHandle handle);
void DWordHashMap_DestructWithCallback (DWordHashMapHandle handle, DWordHashMap_DestructCallback fCallback);
