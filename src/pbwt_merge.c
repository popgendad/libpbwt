#include "pbwt.h"

pbwt_t *pbwt_merge(pbwt_t *b1, pbwt_t *b2)
{
    int ret = 0;
    size_t i = 0;
    pbwt_t *bm = NULL;

    /* Check if two pbwts have same number of sites */
    if (b1->nsite != b2->nsite)
    {
        return NULL;
    }

    /* Check concordance of RSIDs */
    for (i = 0; i < b1->nsite; ++i)
    {
        if (strcmp(b1->rsid[i], b2->rsid[i]) != 0)
        {
            return NULL;
        }
    }

    /* Check if pbwts are compressed */
    if (b1->is_compress == TRUE)
    {
        ret = pbwt_uncompress(b1);
    }

    if (ret < 0)
    {
        return NULL;
    }

    if (b2->is_compress == TRUE)
    {
        ret = pbwt_uncompress(b2);
    }

    if (ret < 0)
    {
        return NULL;
    }

    /* Initialize new pbwt */
    bm = pbwt_init(b1->nsite, b1->nsam + b2->nsam);

    /* Copy genotype data */
    memcpy(bm->data, b1->data, b1->datasize * sizeof(unsigned char));
    memcpy(bm->data + b1->datasize, b2->data, b2->datasize * sizeof(unsigned char));

    /* Copy sample information */
    for (i = 0; i < b1->nsam; ++i)
    {
        bm->sid[i] = strdup(b1->sid[i]);
        bm->reg[i] = strdup(b1->reg[i]);
    }

    for (i = 0; i < b2->nsam; ++i)
    {
        size_t j = b1->nsam + i;
        bm->sid[j] = strdup(b2->sid[i]);
        bm->reg[j] = strdup(b2->reg[i]);
    }

    /* Copy site information */
    for (i = 0; i < b1->nsite; ++i)
    {
        bm->rsid[i] = strdup(b1->rsid[i]);
        bm->cm[i] = b1->cm[i];
        bm->chr[i] = strdup(b1->chr[i]);
    }

    /* Deallocate original pbwts */
    pbwt_destroy(b1);
    pbwt_destroy(b2);

    return bm;
}
