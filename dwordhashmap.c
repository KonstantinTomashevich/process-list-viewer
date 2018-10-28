#include "dwordhashmap.h"
#include "array.h"

ARRAY_H (DWordPair)
typedef struct
{
    DWordPairArrayHandle hBuffer;
} DWordHashBucket;

ARRAY_H (DWordHashBucket)
typedef struct
{
    DWordHashBucketArrayHandle hBuckets;
} DWordHashMap;

DWordHashMapHandle DWordHashMap_Create (unsigned long uBuckets)
{
    DWordHashMap *map = malloc (sizeof (DWordHashMap));
    map->hBuckets = DWordHashBucketArray_Create (uBuckets);

    for (unsigned long uIndex = 0; uIndex < uBuckets; ++uIndex)
    {
        DWordHashBucket bucket;
        bucket.hBuffer = DWordPairArray_Create (0);
        DWordHashBucketArray_Push (map->hBuckets, &bucket);
    }

    return (DWordHashMapHandle) map;
}

void DWordHashMap_InsertValue (DWordHashMapHandle handle, DWORD dKey, DWORD dValue)
{
    DWordHashMap *map = (DWordHashMap *) handle;
    unsigned long hash = dKey % DWordHashBucketArray_Size (map->hBuckets);
    DWordPair dataPair = {dKey, dValue};
    DWordPairArray_Push (DWordHashBucketArray_Index (map->hBuckets, hash)->hBuffer, &dataPair);
}

DWORD *DWordHashMap_GetValue (DWordHashMapHandle handle, DWORD dKey)
{
    DWordHashMap *map = (DWordHashMap *) handle;
    unsigned long hash = dKey % DWordHashBucketArray_Size (map->hBuckets);
    DWordPairArrayHandle hBucketArray = DWordHashBucketArray_Index (map->hBuckets, hash)->hBuffer;
    unsigned long uBucketSize = DWordPairArray_Size (hBucketArray);

    for (unsigned long uIndex = 0; uIndex < uBucketSize; ++uIndex)
    {
        DWordPair *pair = DWordPairArray_Index (hBucketArray, uIndex);
        if (pair->dKey == dKey)
        {
            return &pair->dValue;
        }
    }

    return NULL;
}

void DWordHashMap_Destruct (DWordHashMapHandle handle)
{
    DWordHashMap *map = (DWordHashMap *) handle;
    unsigned long uBucketsCount = DWordHashBucketArray_Size (map->hBuckets);

    for (unsigned long uIndex = 0; uIndex < uBucketsCount; ++uIndex)
    {
        DWordPairArray_Destruct (DWordHashBucketArray_Index (map->hBuckets, uIndex)->hBuffer);
    }

    DWordHashBucketArray_Destruct (map->hBuckets);
    free (map);
}

void DWordHashMap_DestructWithCallback(DWordHashMapHandle handle, DWordHashMap_DestructCallback fCallback)
{
    DWordHashMap *map = (DWordHashMap *) handle;
    unsigned long uBucketsCount = DWordHashBucketArray_Size (map->hBuckets);

    for (unsigned long uIndex = 0; uIndex < uBucketsCount; ++uIndex)
    {
        DWordPairArrayHandle hBucketArray = DWordHashBucketArray_Index (map->hBuckets, uIndex)->hBuffer;
        unsigned long uBucketSize = DWordPairArray_Size (hBucketArray);

        for (unsigned long uItemIndex = 0; uItemIndex < uBucketSize; ++uItemIndex)
        {
            fCallback (DWordPairArray_Index (hBucketArray, uItemIndex));
        }

        DWordPairArray_Destruct (hBucketArray);
    }

    DWordHashBucketArray_Destruct (map->hBuckets);
    free (map);
}

ARRAY_C (DWordPair)
ARRAY_C (DWordHashBucket)
