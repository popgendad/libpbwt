#include <stdio.h>
#include "pbwt.h"

void
pbwt_print_match (pbwt_t *b, match_t *root)
{
    if (root == NULL)
        return;

    pbwt_print_match (b, root->left);

    printf ("%s\t%s\t%zu\t%zu\t%1.5lf\t%1.5lf\n",
            b->sid[root->first], b->sid[root->second],
            root->begin, root->end, b->cm[root->begin], b->cm[root->end]);

    pbwt_print_match (b, root->right);
}
