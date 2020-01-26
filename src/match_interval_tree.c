#include "pbwt.h"

int match_overlap(const size_t, const size_t, const size_t, const size_t);
node_t *match_new(const size_t, const size_t);

size_t match_count(pbwt_t *b, node_t *node, double *al)
{
    static size_t count = 0;

    if (node == NULL)
    {
        return count;
    }

    match_count(b, node->left, al);
    *al += b->cm[node->end] - b->cm[node->begin];
    count++;
    match_count(b, node->right, al);

    return count;
}

node_t *match_insert(node_t *node, const size_t first, const size_t second,
                     const size_t begin, const size_t end)
{
    /* Add a new leaf node */
    if (node == NULL)
    {
        node = match_new(begin, end);
    }

    /* Add to count if interval exists */
    if (node->begin == begin && node->end == end)
    {
        ++node->count;
        return node;
    }

    /* Traverse the tree */
    if (begin < node->begin)
    {
        node->left = match_insert(node->left, first, second, begin, end);
    }
    else
    {
        node->right = match_insert(node->right, first, second, begin, end);
    }

    if (node->max < end)
    {
        node->max = end;
    }

    return node;
}

node_t *match_new(const size_t begin, const size_t end)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    if (node == NULL)
    {
        return NULL;
    }
    node->begin = begin;
    node->end = end;
    node->max = end;
    node->count = 1;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int match_overlap(const size_t begin1, const size_t end1, const size_t begin2, const size_t end2)
{
    return begin1 <= end2 && begin2 <= end1 ? 1 : 0;
}

void tree_print(pbwt_t *b, node_t *root)
{
    if (root == NULL)
    {
        return;
    }
    tree_print(b, root->left);
    printf("%zu\t%zu\t%zu\t%1.5lf\t%1.5lf\n", root->begin, root->end, root->count, b->cm[root->begin], b->cm[root->end]);
    tree_print(b, root->right);
}
