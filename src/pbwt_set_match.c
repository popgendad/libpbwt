#include "pbwt.h"

int pbwt_set_match(pbwt_t *b, const double minlen,
                   void (*report)(pbwt_t *b, const size_t first, const size_t second, const size_t begin, const size_t end))
{
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    size_t da = 0;
    size_t db = 0;
    size_t ia = 0;
    size_t ib = 0;
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
        div[0] = i + 1;
        div[b->nsam] = i + 1;

        for (j = 0; j < b->nsam; ++j)
        {
            int m = j - 1;
            int n = j + 1;
            int mc = 0;

            if (div[j] <= div[j+1])
            {
                while (div[m+1] <= div[j])
                {
                    if (b->data[TWODCORD(ppa[m], b->nsite, i)] ==
                        b->data[TWODCORD(ppa[j], b->nsite, i)] && i < b->nsite - 1)
                    {
                        mc = 1;
                        break;
                    }
                    m -= 1;
                }
            }

            if (mc)
            {
                continue;
            }

            if (div[j] >= div[j+1])
            {
                while (div[n] <= div[j+1])
                {
                    if (b->data[TWODCORD(ppa[n], b->nsite, i)] ==
                        b->data[TWODCORD(ppa[j], b->nsite, i)] && i < b->nsite - 1)
                    {
                        mc = 1;
                        break;
                    }
                    n += 1;
                }
            }

            if (mc)
            {
                continue;
            }

            for (k = m + 1; k < j; ++k)
            {
                /* Evaluate whether match should be recorded */
                double match_dist = b->cm[i] - b->cm[div[j]];

                if (div[j] < i && match_dist >= minlen)
                {
                    if (ppa[j] < ppa[k])
                    {
                        (*report)(b, ppa[j], ppa[k], div[j], i);
                    }
                    else
                    {
                        (*report)(b, ppa[k], ppa[j], div[j], i);
                    }
                }
            }

            for (k = j + 1; k < n; ++k)
            {
                /* Evaluate whether match should be recorded */
                double match_dist = b->cm[i] - b->cm[div[j+1]];

                if (div[j+1] < i && match_dist >= minlen)
                {
                    if (ppa[j] < ppa[k])
                    {
                        (*report)(b, ppa[j], ppa[k], div[j+1], i);
                    }
                    else
                    {
                        (*report)(b, ppa[k], ppa[j], div[j+1], i);
                    }
                }
            }
        }

        if (i < b->nsite - 1)
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
