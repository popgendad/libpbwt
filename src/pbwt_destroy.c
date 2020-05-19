#include "pbwt.h"

/* Local function declaration */
static void match_destroy(node_t *);

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

        if (b->chr != NULL)
        {
            for (i = 0; i < b->nsite; ++i)
            {
                if (b->chr[i] != NULL)
                {
                    free(b->chr[i]);
                }
            }
            free(b->chr);
        }

        if (b->cm != NULL)
        {
            free(b->cm);
        }

        if (b->intree != NULL)
        {
            match_destroy(b->intree);
        }

        if (b->reghash != NULL)
        {
            for (i = 0; i != kh_end(b->reghash); ++i)
            {
                if (kh_exist(b->reghash, i))
                {
                    free((char *)kh_key(b->reghash, i));
                }
            }
            kh_destroy(floats, b->reghash);
        }

        if (b->cmatrix != NULL)
        {
            for (i = 0; i < b->nsam - b->nref; ++i)
            {
                free(b->cmatrix[i]);
            }
            free(b->cmatrix);
        }

        if (b->nmatrix != NULL)
        {
            for (i = 0; i < b->nsam; ++i)
            {
                free(b->nmatrix[i]);
            }
            free(b->nmatrix);
        }

        free(b);
    }

    return;
}


static void match_destroy(node_t *root)
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
