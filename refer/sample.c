#include "pbwt.h"

/* globals */

static DICT *sampleDict;
static DICT *populationDict;
static Array samples;

/* functions */

int
sampleInit (void)
{
    sampleDict = dictCreate(4096);
    populationDict = dictCreate(64);
    samples = arrayCreate(4096, Sample);
    array(samples, 0, Sample).nameD = 0; /* so that all read samples are non-zero */
    return 0;
}

int
sampleDestroy (void)
{
    if (sampleDict)
        dictDestroy(sampleDict);
    if (populationDict)
        dictDestroy(populationDict);
    if (samples)
        arrayDestroy(samples);
    return 0;
}

int
sampleAdd (char *name, char *father, char *mother, char *pop)
{
    int k = 0;
    if (dictAdd (sampleDict, name, &k))
        arrayp(samples, k, Sample)->nameD = k;

    return k;
}

Sample *
sample (PBWT *p, int i) 
{
    i = arr(p->samples, i, int);
    if (i >= arrayMax(samples))
        die ("sample index %d out of range %ld", i, arrayMax(samples));
    return arrp(samples, i, Sample);
}

char *
sampleName (Sample *s)
{
    return dictName (sampleDict, s->nameD);
}

char *
popName (Sample *s)
{
    return dictName (populationDict, s->popD);
}
