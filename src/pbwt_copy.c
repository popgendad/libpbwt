#include <stdlib.h>
#include <string.h>
#include "pbwt.h"


pbwt_t *pbwt_copy(pbwt_t *b)
{
    int ret = 0;
    size_t i = 0;
    pbwt_t *bc = NULL;

    /* Initialize new pbwt */
    bc = pbwt_init(b->nsite, b->nsam);

    if (b->is_compress)
    {
        ret = pbwt_uncompress(b);
    }

    if (ret > 0)
    {
        return NULL;
    }

    /* Copy genotype data */
    memcpy(bc->data, b->data, b->datasize * sizeof(unsigned char));

    /* Copy sample information */
    for (i = 0; i < b->nsam; ++i)
    {
        bc->sid[i] = strdup(b->sid[i]);
        bc->reg[i] = strdup(b->reg[i]);
    }

    /* Copy site information */
    for (i = 0; i < b->nsite; ++i)
    {
        bc->rsid[i] = strdup(b->rsid[i]);
        bc->cm[i] = b->cm[i];
        bc->chr[i] = b->chr[i];
    }

    return bc;
}
