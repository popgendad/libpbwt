#include "pbwt.h"

size_t *pbwt_build(const pbwt_t *b)
{
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    size_t ia = 0;
    size_t ib = 0;
    size_t da = 0;
    size_t db = 0;
    size_t *ppa = NULL;
    size_t *div = NULL;
    size_t *ara = NULL;
    size_t *arb = NULL;
    size_t *ard = NULL;
    size_t *are = NULL;

    /* Allocate heap memory for prefix and divergence arrays */
    div = (size_t *)malloc((b->nsam + 1) * sizeof(size_t));
    if (div == NULL)
    {
        return NULL;
    }

    ppa = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (ppa == NULL)
    {
        return NULL;
    }

    /* Initialize prefix and divergence arrays */
    for (i = 0; i < b->nsam; ++i)
    {
        div[i] = 0;
        ppa[i] = i;
    }

    ara = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (ara == NULL)
    {
        return NULL;
    }

    arb = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (arb == NULL)
    {
        return NULL;
    }

    ard = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (ard == NULL)
    {
        return NULL;
    }

    are = (size_t *)malloc(b->nsam * sizeof(size_t));
    if (are == NULL)
    {
        return NULL;
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
        if (i < b->nsite - 1)
        {
            for (j = 0; j < ia; ++j)
            {
                ppa[j] = ara[j];
                div[j] = ard[j];
            }
            for (j = 0, k = ia; j < ib; ++j, ++k)
            {
                ppa[k] = arb[j];
                div[k] = are[j];
            }
        }
    }

    /* Free allocated memory */
    free(div);
    free(ara);
    free(arb);
    free(ard);
    free(are);

    return ppa;
}
