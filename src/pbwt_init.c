#include "pbwt.h"

pbwt_t *pbwt_init(const size_t nsite, const size_t nsam)
{
    size_t i = 0;
    pbwt_t *b = NULL;

    /* Allocate heap memory for the pwbt data structure */
    b = (pbwt_t *)malloc(sizeof(pbwt_t));
    if (b == NULL)
    {
        return NULL;
    }

    b->nsite = nsite;
    b->nsam = nsam;
    b->datasize = nsite * nsam;
    b->is_compress = FALSE;
    b->intree = NULL;

    b->is_query = (unsigned char *)malloc(nsam * sizeof(unsigned char));
    if (b->is_query == NULL)
    {
        pbwt_destroy(b);
        return NULL;
    }

    b->sid = (char **)malloc(nsam * sizeof(char *));
    if (b->sid == NULL)
    {
        pbwt_destroy(b);
        return NULL;
    }

    b->reg = (char **)malloc(nsam * sizeof(char *));
    if (b->reg == NULL)
    {
        pbwt_destroy(b);
        return NULL;
    }

    b->data = (unsigned char *)malloc(nsite * nsam * sizeof(unsigned char));
    if (b->data == NULL)
    {
        pbwt_destroy(b);
        return NULL;
    }

    b->chr = (char **)malloc(nsite * sizeof(char *));
    if (b->chr == NULL)
    {
        pbwt_destroy(b);
        return NULL;
    }

    b->cm = (double *)malloc(nsite * sizeof(double));
    if (b->cm == NULL)
    {
        pbwt_destroy(b);
        return NULL;
    }

    b->rsid = (char **)malloc(nsite * sizeof(char *));
    if (b->rsid == NULL)
    {
        pbwt_destroy(b);
        return NULL;
    }

    /* Initialize values for prefix and divergence arrays */
    for (i = 0; i < nsam; ++i)
    {
        b->is_query[i] = FALSE;
    }

    return b;
}
