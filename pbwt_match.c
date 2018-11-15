#include <stdlib.h>
#include <stdio.h>
#include "pbwt.h"

static int add_match (match_t **, const size_t, const size_t, const size_t, const size_t);

match_t *
pbwt_match (pbwt_t *b, const size_t query_index, const size_t minlen)
{
    size_t i;
    size_t j;
    size_t k;
    size_t *sdiv;
    size_t *jppa;
    match_t *mlist;

    mlist = NULL;

    /* Allocate heap memory for prefix and divergence arrays */
    sdiv = (size_t *) malloc ((b->nsam + 1) * sizeof(size_t));
    if (sdiv == NULL)
    {
        perror ("libpbwt [ERROR]");
        return NULL;
    }
    jppa = (size_t *) malloc (b->nsam * sizeof(size_t));
    if (jppa == NULL)
    {
        perror ("libpbwt [ERROR]");
        return NULL;
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
                        b->data[TWODCORD(jppa[j], b->nsite, i)])
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
                        b->data[TWODCORD(jppa[j], b->nsite, i)])
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
                if (sdiv[j] < i && i - sdiv[j] >= minlen)
                {
                    add_match (&mlist, jppa[j], jppa[k], sdiv[j], i);
                }
            }

            for (k = j + 1; k < n; ++k)
            {
                if (sdiv[j+1] < i && i - sdiv[j+1] >= minlen)
                {
                    add_match (&mlist, jppa[j], jppa[k], sdiv[j+1], i);
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

    return mlist;
}

static int
add_match (match_t **head, const size_t first, const size_t second,
           const size_t begin, const size_t end)
{
    match_t *new_match;

    /* Allocate heap memory for new match */
    new_match = (match_t *) malloc (sizeof(match_t));
    if (new_match == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }

    /* Populate the match data */
    new_match->first = first;
    new_match->second = second;
    new_match->begin = begin;
    new_match->end = end;
    new_match->next = (*head);
    (*head) = new_match;

    return 0;
}
