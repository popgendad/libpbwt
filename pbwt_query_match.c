#include <stdio.h>
#include <stdlib.h>
#include "pbwt.h"

int
pbwt_query_match (pbwt_t *b, const double minlen)
{
    size_t i;
    size_t j;
    size_t k;
    size_t *sdiv;
    size_t *jppa;
    match_t *intree;

    intree = NULL;

    /* Allocate heap memory for prefix and divergence arrays */
    sdiv = (size_t *) malloc ((b->nsam + 1) * sizeof(size_t));
    if (sdiv == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }
    jppa = (size_t *) malloc (b->nsam * sizeof(size_t));
    if (jppa == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }

    /* Initialize prefix and divergence arrays */
    for (i = 0; i < b->nsam; ++i)
    {
        sdiv[i] = 0;
        jppa[i] = i;
    }

    for (i = 0; i < b->nsite; ++i)
    {
        sdiv[0] = i + 1;
        sdiv[b->nsam] = i + 1;

        for (j = 0; j < b->nsam; ++j)
        {
            int m = j - 1;
            int n = j + 1;
            int mc = 0;

            if (sdiv[j] <= sdiv[j+1])
            {
                while (sdiv[m+1] <= sdiv[j])
                {
                    if (b->data[TWODCORD(jppa[m], b->nsite, i)] ==
                        b->data[TWODCORD(jppa[j], b->nsite, i)] && i < b->nsite - 1)
                    {
                        mc = 1;
                        break;
                    }
                    m -= 1;
                }
            }

            if (mc)
                continue;

            if (sdiv[j] >= sdiv[j+1])
            {
                while (sdiv[n] <= sdiv[j+1])
                {
                    if (b->data[TWODCORD(jppa[n], b->nsite, i)] ==
                        b->data[TWODCORD(jppa[j], b->nsite, i)] && i < b->nsite - 1)
                    {
                        mc = 1;
                        break;
                    }
                    n += 1;
                }
            }

            if (mc)
                continue;

            for (k = m + 1; k < j; ++k)
            {
                /* Evaluate whether match should be recorded */
                double match_dist = (b->cm[i] - b->cm[sdiv[j]]) / (b->cm[b->nsite-1] - b->cm[0]);
                if (b->is_query[jppa[j]] == TRUE && b->is_query[jppa[k]] == FALSE &&
                    sdiv[j] < i && match_dist >= minlen)
                {
                    if (jppa[j] < jppa[k])
                        intree = match_insert (intree, jppa[j], jppa[k], sdiv[j], i);
                    else
                        intree = match_insert (intree, jppa[k], jppa[j], sdiv[j], i);
                }
            }

            for (k = j + 1; k < n; ++k)
            {
                /* Evaluate whether match should be recorded */
                double match_dist = (b->cm[i] - b->cm[sdiv[j+1]]) / (b->cm[b->nsite-1] - b->cm[0]);
                if (b->is_query[jppa[j]] == TRUE && b->is_query[jppa[k]] == FALSE &&
                    sdiv[j+1] < i && match_dist >= minlen)
                {
                    if (jppa[j] < jppa[k])
                        intree = match_insert (intree, jppa[j], jppa[k], sdiv[j+1], i);
                    else
                        intree = match_insert (intree, jppa[k], jppa[j], sdiv[j+1], i);
                }
            }
        }

        if (i < b->nsite - 1)
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

                ix = jppa[j];
                ms = sdiv[j];
                if (ms > da)
                    da = ms;
                if (ms > db)
                    db = ms;

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

            /* Concatenate arrays */
            for (j = 0; j < ia; ++j)
            {
                jppa[j] = ara[j];
                sdiv[j] = ard[j];
            }

            for (j = 0, k = ia; j < ib; ++j, ++k)
            {
                jppa[k] = arb[j];
                sdiv[k] = are[j];
            }

            free (ara);
            free (arb);
            free (ard);
            free (are);
        }
    }

    free (sdiv);
    free (jppa);

    b->match = intree;

    return 0;
}
