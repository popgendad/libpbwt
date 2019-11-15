#include <stdio.h>
#include <string.h>
#include "pbwt.h"

int
pbwt_find_match (pbwt_t *b, const size_t minlen)
{
    size_t i;
    size_t j;
    size_t k = 0;
    size_t kk = 0;
    size_t *mdiv;
    size_t *mppa;
    match_t *intree;

    intree = NULL;

    /* Allocate heap memory for prefix and divergence arrays */
    mdiv = (size_t *) malloc ((b->nsam + 1) * sizeof(size_t));
    if (mdiv == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }
    mppa = (size_t *) malloc (b->nsam * sizeof(size_t));
    if (mppa == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }

    /* Initialize prefix and divergence arrays */
    for (i = 0; i < b->nsam; ++i)
    {
        mdiv[i] = 0;
        mppa[i] = i;
    }

    for (i = 0; i < b->nsite; ++i)
    {
        size_t da;
        size_t db;
        size_t ia;
        size_t ib;
        size_t *ara;
        size_t *arb;
        size_t *ard;
        size_t *are;

        ara = (size_t *) malloc (b->nsam * sizeof(size_t));
        arb = (size_t *) malloc (b->nsam * sizeof(size_t));
        ard = (size_t *) malloc (b->nsam * sizeof(size_t));
        are = (size_t *) malloc (b->nsam * sizeof(size_t));

        da = i + 1;
        db = i + 1;
        ia = 0;
        ib = 0;

        for (j = 0; j < b->nsam; ++j)
        {
            size_t ix;
            size_t ms;

            ix = mppa[j];
            ms = mdiv[j];

            if (ms > i - minlen)
            {
                size_t x;
                size_t y;
                if (ia > 0 && ib > 0)
                {
                    for (x = k; x < j; ++x)
                    {
                        for (y = x + 1, kk = 0; y < j; ++y)
                        {
                            if (mdiv[y] > kk)
                                kk = mdiv[y];
                            unsigned char aa = b->data[TWODCORD(x, b->nsite, i)];
                            unsigned char bb = b->data[TWODCORD(y, b->nsite, i)];
                            if (aa != bb)
                                intree = match_insert (intree, mppa[x], mppa[y], kk, i);
                        }
                    }
                    ia = 0;
                    ib = 0;
                    k = j;
                }
            }
            if (b->data[TWODCORD(ix, b->nsite, i)] == '0')
            {
                ara[ia] = ix;
                ard[ia] = da;
                da = 0;
                ++ia;
            }
            else
            {
                arb[ib] = ix;
                are[ib] = db;
                db = 0;
                ++ib;
            }
        }
/*        if (ia > 0 && ib > 0)
        {
            size_t x;
            size_t y;
            for (x=0; x<ia; ++x)
                for (y=0; y<ib; ++y)
                    intree = match_insert (intree, ma[x], mb[y], i, b->nsite);
        }*/
        if (i < b->nsite - 1)
        {
            /* Concatenate arrays */
            for (j = 0; j < ia; ++j)
            {
                mppa[j] = ara[j];
                mdiv[j] = ard[j];
            }

            for (j = 0, k = ia; j < ib; ++j, ++k)
            {
                mppa[k] = arb[j];
                mdiv[k] = are[j];
            }
        }

        free (ara);
        free (arb);
        free (ard);
        free (are);
    }

    free (mdiv);
    free (mppa);

    b->match = intree;

    return 0;
}
