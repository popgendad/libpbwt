#include <string.h>
#include "pbwt.h"

#define MININT 10

int pbwt_all_query_match(pbwt_t *b, const double minlen)
{
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    size_t kk = 0;
    size_t da = 0;
    size_t db = 0;
    size_t ia = 0;
    size_t ib = 0;
    size_t na = 0;
    size_t nb = 0;
    size_t *mdiv = NULL;
    size_t *mppa = NULL;
    size_t *ara = NULL;
    size_t *arb = NULL;
    size_t *ard = NULL;
    size_t *are = NULL;
    match_t *intree = NULL;

    /* Allocate heap memory for prefix and divergence arrays */
    mdiv = (size_t *)malloc((b->nsam + 1) * sizeof(size_t));
    if (mdiv == NULL)
    {
        return -1;
    }

    mppa = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (mppa == NULL)
    {
        return -1;
    }

    /* Initialize prefix and divergence arrays */
    for (i = 0; i < b->nsam; ++i)
    {
        mdiv[i] = 0;
        mppa[i] = i;
    }

    ara = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (ara == NULL)
    {
        return -1;
    }
    arb = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (arb == NULL)
    {
        return -1;
    }
    ard = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (ard == NULL)
    {
        return -1;
    }
    are = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (are == NULL)
    {
        return -1;
    }

    for (i = 0; i <= b->nsite; ++i)
    {
        memset(ara, 0, b->nsam);
        memset(arb, 0, b->nsam);
        memset(ard, 0, b->nsam);
        memset(are, 0, b->nsam);

        da = i + 1;
        db = i + 1;
        ia = 0;
        ib = 0;

        for (j = 0; j < b->nsam; ++j)
        {
            size_t ix = 0;
            size_t ms = 0;

            ix = mppa[j];
            ms = mdiv[j];

            if (ms > i - MININT)
            {
                size_t x = 0;
                size_t y = 0;

                if (na > 0 && nb > 0)
                {
                    for (x = k; x < j; ++x)
                    {
                        for (y = x + 1, kk = 0; y < j; ++y)
                        {
                            if (mdiv[y] > kk)
                            {
                                kk = mdiv[y];
                            }
                            unsigned char aa = b->data[TWODCORD(mppa[x], b->nsite, i)];
                            unsigned char bb = b->data[TWODCORD(mppa[y], b->nsite, i)];
                            if (aa != bb && b->cm[i] - b->cm[kk] > minlen &&
                            	((b->is_query[mppa[x]] == TRUE && b->is_query[mppa[y]] == FALSE) ||
                            	(b->is_query[mppa[x]] == FALSE && b->is_query[mppa[y]] == TRUE)))
                            {
                                intree = match_insert(intree, mppa[x], mppa[y], kk, i);
                            }
                        }
                    }
                    na = 0;
                    nb = 0;
                    k = j;
                }
            }

            if (ms > da)
            {
                da = ms;
            }

            if (ms > db)
            {
                db = ms;
            }

            if (b->data[TWODCORD(ix, b->nsite, i)] == '0')
            {
                ara[ia] = ix;
                ard[ia] = da;
                da = 0;
                ++na;
                ++ia;
            }
            else
            {
                arb[ib] = ix;
                are[ib] = db;
                db = 0;
                ++nb;
                ++ib;
            }
        }

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
    }

    free(mdiv);
    free(mppa);
    free(ara);
    free(arb);
    free(ard);
    free(are);

    b->match = intree;

    return 0;
}