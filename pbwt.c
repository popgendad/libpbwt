#include <stdlib.h>
#include "pbwt.h"

int
pbwtInit (void) 
{
    variationDict = dictCreate (32);
    pack3init (); 
    sampleInit ();
    return 0;
}

PBWT *
pbwtCreate (int nsam, int nsite)
{
    int i = 0;
    PBWT *p = NULL;
    
    p = malloc(sizeof(PBWT));
    p->nsam = nsam;
    p->nsite = nsite;
    p->aFstart = malloc(nsam * sizeof(int));
    for (i = 0; i < nsam; ++i)
        p->aFstart[i] = i;

  return p;
}

int
pbwtDestroy (PBWT *p)
{
  if (p->chr)
    free (p->chr);
  if (p->sites)
    arrayDestroy (p->sites);
  if (p->samples)
    arrayDestroy (p->samples);
  if (p->yz)
    arrayDestroy (p->yz);
  if (p->zz)
    arrayDestroy (p->zz);
  if (p->aFstart)
    free (p->aFstart);
  if (p->aFend)
    free (p->aFend);
  if (p->aRstart)
    free (p->aRstart);
  if (p->aRend)
    free (p->aRend);
  if (p->missingOffset)
    arrayDestroy (p->missingOffset);
  if (p->zMissing)
    arrayDestroy (p->zMissing);
  if (p->dosageOffset)
    arrayDestroy (p->dosageOffset);
  if (p->zDosage)
    arrayDestroy (p->zDosage);
  free (p);

  return 0;
}

static int p3decode[128];
#define ENCODE_MAX1 64		          /* ~64 */
#define ENCODE_MAX2 ((95-63) << 6)    /* ~1k - is this 32 or 31?*/
#define ENCODE_MAX3 ((127-96) << 11)  /* ~64k - ditto */

static int
pack3init (void)
{
    int i = 0;
    for (i = 0; i < 64; ++i)
        p3decode[i] = i;
    for (i = 64; i < 96; ++i)
        p3decode[i] = (i - 64) << 6;
    for (i = 96; i < 128; ++i)
        p3decode[i] = (i - 96) << 11;
    return 0;
}
