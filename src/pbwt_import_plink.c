#include <string.h>
#include <plink_lite.h>
#include "pbwt.h"


pbwt_t *pbwt_import_plink(const char *instub)
{
    size_t i = 0;
    size_t len = 0;
    size_t q = 0;
    khint_t z = 0;
    plink_t *p = NULL;
    pbwt_t *b = NULL;

    /* Initialize plink data structure */
    p = plink_init(instub, 1, 1);
    if (p == NULL)
    {
        return NULL;
    }

    /* Initialize pbwt structure */
    b = pbwt_init(p->nsnp, 2 * p->nsam);
    if (b == NULL)
    {
        return NULL;
    }

    /* Iterate through all samples in the fam/reg */
    for (i = 0; i < p->nsam; ++i)
    {
        memcpy(&b->data[TWODCORD(2*i, b->nsite, 0)],
               hap2uchar(p, i, 0),
               b->nsite * sizeof(unsigned char));

        memcpy(&b->data[TWODCORD(2*i+1, b->nsite, 0)],
               hap2uchar(p, i, 1),
               b->nsite * sizeof(unsigned char));
        len = strlen(p->fam[i].iid);
        b->sid[2*i] = (char *)malloc(len + 3);
        strcpy(b->sid[2*i], p->fam[i].iid);
        strcat(b->sid[2*i], ".1");
        b->sid[2*i+1] = (char *)malloc(len + 3);
        strcpy(b->sid[2*i+1], p->fam[i].iid);
        strcat(b->sid[2*i+1], ".2");
        z = kh_get(integer, p->reg_index, p->fam[i].iid);
        q = kh_value(p->reg_index, z);
        b->reg[2*i] = strdup(p->reg[q].reg);
        b->reg[2*i+1] = strdup(p->reg[q].reg);
    }

    /* Iterate through all sites in the bim */
    for (i = 0; i < p->nsnp; ++i)
    {
        b->rsid[i] = strdup(p->bim[i].rsid);
        b->cm[i] = p->bim[i].cM;
        b->chr[i] = p->bim[i].ch;
    }

    return b;
}
