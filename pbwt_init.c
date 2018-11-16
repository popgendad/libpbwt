#include <stdio.h>
#include <stdlib.h>
#include "pbwt.h"

pbwt_t *
pbwt_init (const size_t nsite, const size_t nsam)
{
    size_t i;
    pbwt_t *b;

    /* Allocate heap memory for the pwbt data structure */
    b = (pbwt_t *) malloc (sizeof(pbwt_t));
    if (b == NULL)
    {
        perror ("libpbwt [ERROR]");
        return NULL;
    }

    b->nsite = nsite;
    b->nsam = nsam;
    b->datasize = nsite * nsam;
    b->is_compress = FALSE;
    b->match = NULL;

    b->ppa = (size_t *) malloc (nsam * sizeof(size_t));
    if (b->ppa == NULL)
    {
        perror ("libpbwt [ERROR]");
        pbwt_destroy (b);
        return NULL;
    }

    b->div = (size_t *) malloc (nsam * sizeof(size_t));
    if (b->div == NULL)
    {
        perror ("libpbwt [ERROR]");
        pbwt_destroy (b);
        return NULL;
    }

    b->sid = (char **) malloc (nsam * sizeof(char *));
    if (b->sid == NULL)
    {
        perror ("libpbwt [ERROR]");
        pbwt_destroy (b);
        return NULL;
    }

    b->reg = (char **) malloc (nsam * sizeof(char *));
    if (b->reg == NULL)
    {
        perror ("libpbwt [ERROR]");
        pbwt_destroy (b);
        return NULL;
    }

    b->data = (unsigned char *) malloc (nsite * nsam * sizeof(unsigned char));
    if (b->data == NULL)
    {
        perror ("libpbwt [ERROR]");
        pbwt_destroy (b);
        return NULL;
    }

    b->rsid = (char **) malloc (nsite * sizeof(char *));
    if (b->rsid == NULL)
    {
        perror ("libpbwt [ERROR]");
        pbwt_destroy (b);
        return NULL;
    }

    /* Initialize values for prefix and divergence arrays */
    for (i = 0; i < nsam; ++i)
    {
        b->ppa[i] = i;
        b->div[i] = 0;
    }

    return b;
}
