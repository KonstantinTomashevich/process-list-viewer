#pragma once
#include <stdlib.h>

#define ARRAY_H(TYPE) \
typedef unsigned long TYPE ## ArrayHandle; \
TYPE ## ArrayHandle TYPE ## Array_Create (unsigned long uInitialBuffer); \
unsigned long TYPE ## Array_Size (TYPE ## ArrayHandle handle); \
TYPE *TYPE ## Array_Index (TYPE ## ArrayHandle handle, unsigned long uIndex); \
void TYPE ## Array_Push (TYPE ## ArrayHandle handle, TYPE *pItem); \
void TYPE ## Array_Destruct (TYPE ## ArrayHandle handle);

#define ARRAY_C(TYPE) \
typedef struct \
{ \
    unsigned long uSize; \
    unsigned long uCapacity; \
    TYPE *pBuffer; \
} TYPE ## Array; \
\
TYPE ## ArrayHandle TYPE ## Array_Create (unsigned long uInitialBuffer) \
{\
    TYPE ## Array *array = calloc (1, sizeof (TYPE ## Array)); \
    array->uCapacity = uInitialBuffer; \
    array->pBuffer = calloc (uInitialBuffer, sizeof (TYPE)); \
    return (TYPE ## ArrayHandle) array; \
} \
\
unsigned long TYPE ## Array_Size (TYPE ## ArrayHandle handle) \
{ \
    TYPE ## Array *array = (TYPE ## Array *) handle; \
    return array->uSize; \
} \
\
TYPE *TYPE ## Array_Index (TYPE ## ArrayHandle handle, unsigned long uIndex) \
{ \
    TYPE ## Array *array = (TYPE ## Array *) handle; \
    return array->pBuffer + uIndex; \
} \
\
void TYPE ## Array_Push (TYPE ## ArrayHandle handle, TYPE *pItem) \
{ \
    TYPE ## Array *array = (TYPE ## Array *) handle; \
    if (array->uSize == array->uCapacity) \
    { \
        array->uCapacity += 1; \
        array->pBuffer = realloc (array->pBuffer, sizeof (TYPE) * array->uCapacity); \
    } \
    \
    array->pBuffer [array->uSize] = *pItem; \
    array->uSize += 1; \
} \
\
void TYPE ## Array_Destruct (TYPE ## ArrayHandle handle) \
{ \
    TYPE ## Array *array = (TYPE ## Array *) handle; \
    free (array->pBuffer); \
    free (array); \
}
