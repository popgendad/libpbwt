#include <stdlib.h>
#include "pbwt.h"

void
pbwt_destroy (pbwt_t *b)
{
    /* Deallocate all heap memory pointed to by pbwt structure */
    if (b != NULL)
    {
        size_t i;
        if (b->sid != NULL)
        {
            for (i = 0; i < b->nsam; ++i)
                if (b->sid[i] != NULL)
                    free (b->sid[i]);
            free (b->sid);
        }
        if (b->reg != NULL)
        {
            for (i = 0; i < b->nsam; ++i)
                if (b->reg[i] != NULL)
                    free (b->reg[i]);
            free (b->reg);
        }
        if (b->rsid != NULL)
        {
            for (i = 0; i < b->nsite; ++i)
                if (b->rsid[i] != NULL)
                    free (b->rsid[i]);
            free (b->rsid);
        }
        if (b->data != NULL)
            free (b->data);
        if (b->div != NULL)
            free (b->div);
        if (b->ppa != NULL)
            free (b->ppa);
        if (b->match != NULL)
        {
            match_t *p;
            match_t *pn;
            p = b->match;
            while (p != NULL)
            {
                pn = b->match->next;
                free (p);
                p = pn;
            }
        }
        free (b);
    }
    return;
}
