#include "pbwt.h"

/* Local function declaration */
static void match_destroy(match_t *);


void pbwt_destroy(pbwt_t *b)
{
    /* Deallocate all heap memory pointed to by pbwt structure */
    if (b != NULL)
    {
        size_t i = 0;

        if (b->sid != NULL)
        {
            for (i = 0; i < b->nsam; ++i)
            {
                if (b->sid[i] != NULL)
                {
                    free(b->sid[i]);
                }
            }
            free(b->sid);
        }

        if (b->reg != NULL)
        {
            for (i = 0; i < b->nsam; ++i)
            {
                if (b->reg[i] != NULL)
                {
                    free(b->reg[i]);
                }
            }
            free(b->reg);
        }

        if (b->rsid != NULL)
        {
            for (i = 0; i < b->nsite; ++i)
            {
                if (b->rsid[i] != NULL)
                {
                    free(b->rsid[i]);
                }
            }
            free(b->rsid);
        }

        if (b->is_query != NULL)
        {
            free(b->is_query);
        }

        if (b->data != NULL)
        {
            free(b->data);
        }

        if (b->div != NULL)
        {
            free(b->div);
        }

        if (b->ppa != NULL)
        {
            free(b->ppa);
        }

        if (b->chr != NULL)
        {
            free(b->chr);
        }

        if (b->cm != NULL)
        {
            free(b->cm);
        }

        if (b->match != NULL)
        {
            match_destroy(b->match);
        }

        free(b);
    }

    return;
}


static void match_destroy(match_t *root)
{
    if (root == NULL)
    {
        return;
    }
    match_destroy(root->left);
    free(root);
    match_destroy(root->right);
    return;
}
