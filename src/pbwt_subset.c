#include <stdio.h>
#include <string.h>
#include "pbwt.h"

pbwt_t *pbwt_subset(pbwt_t *b, const char *reg)
{
    /* Check to make sure the pbwt has region information */
    if (b->reg == NULL)
    {
        return NULL;
    }

    size_t i = 0;
    size_t j = 0;
    size_t nhap_read = 0;
    size_t nhap_write = 0;
    pbwt_t *p = NULL;

    /* Count number of haplotypes within region */
    for (i = 0; i < b->nsam; ++i)
    {
        if (strcmp(b->reg[i], reg) == 0)
        {
            ++nhap_read;
        }
    }

    /* Check to make sure we have non-null subset */
    if (nhap_read == 0)
    {
        return NULL;
    }

    /* Initialize new pbwt_t structure */
    p = pbwt_init(b->nsite, nhap_read);
    if (p == NULL)
    {
        return NULL;
    }

    /* Copy subset data to new pbwt_t structure */
    for (i = 0; i < b->nsam; ++i)
    {
        if (strcmp(b->reg[i], reg) == 0)
        {
            /* Buffer overflow check */
            if (nhap_write > nhap_read)
            {
                return NULL;
            }

            /* Copy sample identifier */
            p->sid[nhap_write] = strdup(b->sid[i]);

            /* Copy region identifier */
            p->reg[nhap_write] = strdup(b->reg[i]);

            /* Copy genotype data */
            memcpy(&(p->data[TWODCORD(nhap_write, p->nsite, 0)]),
                   &(b->data[TWODCORD(i, b->nsite, 0)]),
                   b->nsite * sizeof(unsigned char));

            /* Iterate number of subset haplotypes */
            ++nhap_write;
        }
    }

    /* Copy site data */
    for (j = 0; j < b->nsite; ++j)
    {
        p->chr[j] = b->chr[j];
        if (b->rsid[j] != NULL)
        {
            p->rsid[j] = strdup(b->rsid[j]);
        }
        p->cm[j] = b->cm[j];
    }

    /* Check to make sure we wrote the right amount of data */
    if (nhap_write < nhap_read)
    {
        return NULL;
    }

    return p;
}


pbwt_t *pbwt_subset_with_query(pbwt_t *b, const char *reg, const size_t query)
{
    /* Check to make sure the pbwt has region information */
    if (b->reg == NULL)
    {
        return NULL;
    }

    size_t i = 0;
    size_t j = 0;
    size_t nhap_read = 0;
    size_t nhap_write = 0;
    pbwt_t *p = NULL;

    /* Count number of haplotypes within region */
    for (i = 0; i < b->nsam; ++i)
    {
        if (strcmp(b->reg[i], reg) == 0 || i == query)
        {
            ++nhap_read;
        }
    }

    /* Check to make sure we have non-null subset */
    if (nhap_read == 0)
    {
        return NULL;
    }

    /* Initialize new pbwt_t structure */
    p = pbwt_init(b->nsite, nhap_read);
    if (p == NULL)
    {
        return NULL;
    }

    /* Copy subset data to new pbwt_t structure */
    for (i = 0; i < b->nsam; ++i)
    {
        if (strcmp(b->reg[i], reg) == 0 || i == query)
        {
            /* Buffer overflow check */
            if (nhap_write > nhap_read)
            {
                return NULL;
            }

            /* Copy sample identifier */
            p->sid[nhap_write] = strdup(b->sid[i]);

            /* Copy region identifier */
            p->reg[nhap_write] = strdup(b->reg[i]);

            /* Copy genotype data */
            memcpy(&(p->data[TWODCORD(nhap_write, p->nsite, 0)]),
                   &(b->data[TWODCORD(i, b->nsite, 0)]),
                   b->nsite * sizeof(unsigned char));

            if (i == query)
            {
                p->is_query[nhap_write] = TRUE;
            }
            else
            {
                p->is_query[nhap_write] = FALSE;
            }

            /* Iterate number of subset haplotypes */
            ++nhap_write;
        }
    }

    /* Copy site data */
    for (j = 0; j < b->nsite; ++j)
    {
        p->chr[j] = b->chr[j];
        if (b->rsid[j] != NULL)
        {
            p->rsid[j] = strdup(b->rsid[j]);
        }
        p->cm[j] = b->cm[j];
    }

    /* Check to make sure we wrote the right amount of data */
    if (nhap_write < nhap_read)
    {
        return NULL;
    }

    return p;
}
