#include <stdio.h>
#include "pbwt.h"

void match_print(pbwt_t *b, match_t *root)
{
    if (root == NULL)
    {
        return;
    }

    match_print(b, root->left);

    printf("%s\t%s\t%s\t%s\t%zu\t%zu\t%1.5lf\t%1.5lf\n",
           b->sid[root->first], b->reg[root->first], b->sid[root->second], b->reg[root->second],
           root->begin, root->end, b->cm[root->begin], b->cm[root->end]);

    match_print(b, root->right);
}

void elementary_print(pbwt_t *b, match_t *root)
{
	if (root == NULL)
	{
		return;
	}

	if (root->left == NULL && root->right == NULL)
	{
		printf("%zu\t%zu\n", root->begin, root->end);
	}

	if (root->left)
	{
		elementary_print(b, root->left);
	}

	if (root->right)
	{
		elementary_print(b, root->right);
	}
}