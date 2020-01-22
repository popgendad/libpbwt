#include <stdio.h>
#include "pbwt.h"

void intree_print(pbwt_t *b, node_t *root)
{
    if (root == NULL)
    {
        return;
    }

    intree_print(b, root->left);

    printf("%s\t%s\t%s\t%s\t%zu\t%zu\t%1.5lf\t%1.5lf\n",
           b->sid[root->first], b->reg[root->first], b->sid[root->second], b->reg[root->second],
           root->begin, root->end, b->cm[root->begin], b->cm[root->end]);

    intree_print(b, root->right);
}
