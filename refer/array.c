#include <stdio.h>
#include <stdlib.h>
#include "pbwt.h"

#define ARRAY_REPORT_MAX 0

static long totalAllocatedMemory = 0;
static int totalNumberCreated = 0;
static int totalNumberActive = 0;
static Array reportArray = 0;

Array
arrayCreate (long n, size_t size)
{
    Array a = (Array)malloc(sizeof(struct ArrayStruct));
    static int isFirst;

    if (isFirst)
    {
        isFirst = 0;
        if (ARRAY_REPORT_MAX)
            reportArray = arrayCreate(512, sizeof(Array);
    }
    if (size <= 0)
    {
        fprintf(stderr, "negative size %zu in arrayCreate", size);
        return NULL;
    }
    if (n < 1)
        n = 1;
    totalAllocatedMemory += n * size;

    a->magic = ARRAY_MAGIC ;
    a->base = (char *)malloc(n * sizeof(size));
    a->dim = n;
    a->max = 0;
    a->size = size;
    a->id = ++totalNumberCreated;
    ++totalNumberActive;
    if (reportArray)
    {
        if (a->id < ARRAY_REPORT_MAX)
            array(reportArray, a->id, Array) = a;
        else
        {
            arrayDestroy (reportArray);
            reportArray = 0;
        }
    }

    return a;
}
