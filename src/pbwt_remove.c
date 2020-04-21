#include "pbwt.h"

pbwt_t *pbwt_remove(pbwt_t *b, const khash_t(integer) *remove)
{
    size_t i = 0;
    size_t j = 0;
    size_t nhap_include = 0;
    size_t nhap_write = 0;
    khint_t k = 0;
    pbwt_t *p = NULL;

    nhap_include = b->nsam - (2 * kh_size(remove));

    /* Initialize new pbwt_t structure */
    p = pbwt_init(b->nsite, nhap_include);
    if (p == NULL)
    {
        return NULL;
    }

    /* Copy subset data to new pbwt_t structure */
    for (i = 0; i < b->nsam; ++i)
    {
        size_t sl = strlen(b->sid[i]) - 2;
        char *query_sid = (char *)malloc(sizeof(sl));
        strncpy(query_sid, b->sid[i], sl);
        k = kh_get(integer, remove, query_sid);
        if (kh_exist(remove, k) && k != kh_end(remove))
        {
        	continue;
        }
        else
        {
            /* Buffer overflow check */
            if (nhap_write > nhap_include)
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
        p->chr[j] = strdup(b->chr[j]);
        if (b->rsid[j] != NULL)
        {
            p->rsid[j] = strdup(b->rsid[j]);
        }
        p->cm[j] = b->cm[j];
    }

    /* Check to make sure we wrote the right amount of data */
    if (nhap_write < nhap_include)
    {
        return NULL;
    }

    return p;
}
