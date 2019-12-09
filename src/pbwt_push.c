#include <stdio.h>
#include <string.h>
#include "pbwt.h"

int pbwt_push(pbwt_t *b, const char *new_sid, const char *new_reg,
              const char *h1, const char *h2)
{
    int has_reg = 0;
    size_t i = 0;
    size_t len = 0;
    size_t new_index1 = 0;
    size_t new_index2 = 0;

    /* Determine if a sample region is provided */
    has_reg = new_reg ? TRUE : FALSE;

    /* Check length of input haplotype sequences */
    if (strlen(h1) != b->nsite || strlen(h2) != b->nsite)
    {
        return -1;
    }

    /* Check that new sample identifier string is not NULL */
    if (new_sid == NULL)
    {
        return -1;
    }

    /* Assign the index positions of the new haplotypes */
    new_index1 = b->nsam;
    new_index2 = b->nsam + 1;

    /* Assign new haplotype sample size */
    b->nsam += 2;

    /* Grow the sample identifier list */
    b->sid = (char **)realloc(b->sid, b->nsam * sizeof(char *));

    /* Add new sample identifier to list */
    len = strlen(new_sid);
    b->sid[new_index1] = (char *)malloc(len * sizeof(char));
    b->sid[new_index2] = (char *)malloc(len * sizeof(char));
    strcpy(b->sid[new_index1], new_sid);
    strcpy(b->sid[new_index2], new_sid);

    /* Grow the region identifier list */
    b->reg = (char **)realloc(b->reg, b->nsam * sizeof(char *));

    /* Add new region identifier to list */
    if (has_reg)
    {
        len = strlen(new_reg);
        b->reg[new_index1] = (char *)malloc(len * sizeof(char));
        b->reg[new_index2] = (char *)malloc(len * sizeof(char));
        strcpy (b->reg[new_index1], new_reg);
        strcpy (b->reg[new_index2], new_reg);
    }

    /* Re-calculate datasize */
    b->datasize = b->nsam * b->nsite;

    /* Grow the binary haplotype matrix */
    b->data = (unsigned char *)realloc(b->data, b->datasize * sizeof(unsigned char));

    /* Copy new binary haplotype data */
    memcpy(&(b->data[TWODCORD(new_index1, b->nsite, 0)]), h1, b->nsite);
    memcpy(&(b->data[TWODCORD(new_index2, b->nsite, 0)]), h2, b->nsite);

    /* Grow the prefix array */
    b->ppa = (size_t *)realloc(b->ppa, b->nsam * sizeof(size_t));

    /* Grow the divergence array */
    b->div = (size_t *)realloc(b->div, b->nsam * sizeof(size_t));

    /* Re-initialize prefix and divergence arrays */
    for (i = 0; i < b->nsam; ++i)
    {
        b->ppa[i] = i;
        b->div[i] = 0;
    }

    return 0;
}
