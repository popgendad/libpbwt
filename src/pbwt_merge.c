#include "pbwt.h"

pbwt_t *pbwt_merge(pbwt_t *bref, pbwt_t *bquery)
{
    int ret = 0;
    size_t i = 0;
    size_t j = 0;
    size_t tot_size = 0;
    pbwt_t *bmerge = NULL;

    /* Check if two pbwts have same number of sites */
    if (bref->nsite != bquery->nsite)
    {
        return NULL;
    }

    /* Check concordance of RSIDs */
    for (i = 0; i < bref->nsite; ++i)
    {
        if (strcmp(bref->rsid[i], bquery->rsid[i]) != 0)
        {
            return NULL;
        }
    }

    /* Check if pbwts are compressed */
    if (bref->is_compress == TRUE)
    {
        ret = pbwt_uncompress(bref);
        if (ret < 0)
        {
            return NULL;
        }
    }

    if (bquery->is_compress == TRUE)
    {
        ret = pbwt_uncompress(bquery);
        if (ret < 0)
        {
            return NULL;
        }
    }

    /* Total sample size of merged pbwt */
    tot_size = bref->nsam + bquery->nsam;

    /* Initialize new pbwt */
    bmerge = pbwt_init(bref->nsite, tot_size);
    bmerge->nref = bref->nsam;

    /* Copy genotype data */
    memcpy(bmerge->data, bref->data, bref->datasize * sizeof(unsigned char));
    memcpy(bmerge->data + bref->datasize, bquery->data, bquery->datasize * sizeof(unsigned char));

    /* Copy sample information */

    /* b1 is reference set */
    for (i = 0; i < bmerge->nref; ++i)
    {
        bmerge->sid[i] = strdup(bref->sid[i]);
        bmerge->reg[i] = strdup(bref->reg[i]);
        bmerge->is_query[i] = FALSE;
    }

    /* b2 is query set */
    for (i = bmerge->nref, j = 0; i < tot_size; ++i, ++j)
    {
        bmerge->sid[i] = strdup(bquery->sid[j]);
        bmerge->reg[i] = strdup(bquery->reg[j]);
        bmerge->is_query[i] = TRUE;
    }

    /* Copy site information */
    for (i = 0; i < bref->nsite; ++i)
    {
        bmerge->rsid[i] = strdup(bref->rsid[i]);
        bmerge->cm[i] = bref->cm[i];
        bmerge->chr[i] = strdup(bref->chr[i]);
    }

    /* Deallocate original pbwts */
    pbwt_destroy(bref);
    pbwt_destroy(bquery);

    return bmerge;
}
