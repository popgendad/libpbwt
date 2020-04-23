#include "pbwt.h"

pbwt_t *pbwt_subset(const pbwt_t *b, const khash_t(integer) *include)
{
    size_t i = 0;
    size_t nhap_include = 0;
    size_t nhap_write = 0;
    khint_t k = 0;
    pbwt_t *p = NULL;

    nhap_include = 2 * kh_size(include);

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
        query_sid[sl] = '\0';
        k = kh_get(integer, include, query_sid);
        if (kh_exist(include, k) && k != kh_end(include))
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
        free(query_sid);
    }

    /* Copy site data */
    for (i = 0; i < b->nsite; ++i)
    {
        p->chr[i] = strdup(b->chr[i]);
        if (b->rsid[i] != NULL)
        {
            p->rsid[i] = strdup(b->rsid[i]);
        }
        memcpy(&(p->cm[i]), &(b->cm[i]), sizeof(double));
    }

    /* Check to make sure we wrote the right amount of data */
    if (nhap_write < nhap_include)
    {
        return NULL;
    }

    return p;
}
