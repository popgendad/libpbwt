#include "pbwt.h"

int match_overlap(const size_t, const size_t, const size_t, const size_t);
node_t *match_new(const size_t, const size_t, const size_t, const size_t);

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
        node_t *nn = match_new(first, second, begin, end);
        if (nn == NULL)
        {
            return NULL;
        }
        else
        {
            return nn;
        }
    }

    /* Don't add if it is a duplicate entry */
    if (node && node->first == first && node->second == second &&
        node->begin == begin && node->end == end)
    {
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

node_t *match_new(const size_t first, const size_t second, const size_t begin, const size_t end)
{
    node_t *node = NULL;

    node = (node_t *)malloc(sizeof(node_t));
    if (node == NULL)
    {
        return NULL;
    }

    node->first = first;
    node->second = second;
    node->begin = begin;
    node->end = end;
    node->max = end;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int match_overlap(const size_t begin1, const size_t end1, const size_t begin2, const size_t end2)
{
    return begin1 <= end2 && begin2 <= end1 ? 1 : 0;
}
