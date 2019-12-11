#include "pbwt.h"

int pbwt_build(pbwt_t *b)
{
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    size_t ia = 0;
    size_t ib = 0;
    size_t da = 0;
    size_t db = 0;
    size_t *ara = NULL;
    size_t *arb = NULL;
    size_t *ard = NULL;
    size_t *are = NULL;

    ara = (size_t *)malloc(b->nsam * sizeof(size_t));
    arb = (size_t *)malloc(b->nsam * sizeof(size_t));
    ard = (size_t *)malloc(b->nsam * sizeof(size_t));
    are = (size_t *)malloc(b->nsam * sizeof(size_t));

    for (i = 0; i < b->nsite; ++i)
    {
        ia = 0;
        ib = 0;
        da = i + 1;
        db = i + 1;

        for (j = 0; j < b->nsam; ++j)
        {
            size_t ix = 0;
            size_t ms = 0;

            ix = b->ppa[j];
            ms = b->div[j];

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
                b->ppa[j] = ara[j];
                b->div[j] = ard[j];
            }
            for (j = 0, k = ia; j < ib; ++j, ++k)
            {
                b->ppa[k] = arb[j];
                b->div[k] = are[j];
            }
        }
    }

    /* Free allocated memory */
    free(ara);
    free(arb);
    free(ard);
    free(are);

    return 0;
}
