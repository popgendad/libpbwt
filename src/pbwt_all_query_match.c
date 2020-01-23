#include <string.h>
#include "pbwt.h"

#define MININT 10

int pbwt_all_query_match(pbwt_t *b, const double minlen,
                         void (*report)(pbwt_t *b, const size_t first, const size_t second, const size_t begin, const size_t end))
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
    size_t *div = NULL;
    size_t *ppa = NULL;
    size_t *ara = NULL;
    size_t *arb = NULL;
    size_t *ard = NULL;
    size_t *are = NULL;

    /* Allocate heap memory for prefix and divergence arrays */
    div = (size_t *)calloc(b->nsam + 1, sizeof(size_t));
    if (div == NULL)
    {
        return -1;
    }

    ppa = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (ppa == NULL)
    {
        return -1;
    }

    /* Initialize prefix and divergence arrays */
    for (i = 0; i < b->nsam; ++i)
    {
        ppa[i] = i;
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

    for (i = 0; i < b->nsite; ++i)
    {
        da = i + 1;
        db = i + 1;
        ia = 0;
        ib = 0;
        memset(ara, 0, b->nsam);
        memset(arb, 0, b->nsam);
        memset(ard, 0, b->nsam);
        memset(are, 0, b->nsam);

        for (j = 0; j < b->nsam; ++j)
        {
            size_t ix = ppa[j];
            size_t ms = div[j];
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
                            if (div[y] > kk)
                            {
                                kk = div[y];
                            }
                            unsigned char aa = b->data[TWODCORD(ppa[x], b->nsite, i)];
                            unsigned char bb = b->data[TWODCORD(ppa[y], b->nsite, i)];

                            /* Check if alleles are different */
                            if (aa == bb)
                            {
                                continue;
                            }
                            else
                            {
                                if (b->cm[i] - b->cm[kk] > minlen &&
                                	((b->is_query[ppa[x]] == TRUE && b->is_query[ppa[y]] == FALSE) ||
                                	(b->is_query[ppa[x]] == FALSE && b->is_query[ppa[y]] == TRUE)))
                                {
                                    (*report)(b, ppa[x], ppa[y], kk, i);
                                }
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
            memcpy(ppa, ara, ia * sizeof(size_t));
            memcpy(div, ard, ia * sizeof(size_t));
            memcpy(ppa + ia, arb, ib * sizeof(size_t));
            memcpy(div + ia, are, ib * sizeof(size_t));
        }
    }

    free(div);
    free(ppa);
    free(ara);
    free(arb);
    free(ard);
    free(are);

    return 0;
}