#include <string.h>
#include <math.h>
#include "pbwt.h"

match_t *match_new(const size_t first, const size_t second, const size_t begin, const size_t end)
{
    match_t *node = NULL;

    node = (match_t *)malloc(sizeof(match_t));
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

int match_adjsearch(pbwt_t *b, match_t *node, adjlist_t *g, size_t qbegin, size_t qend)
{
    if (node == NULL)
    {
        return 0;
    }

    if (match_overlap(qbegin, qend, node->begin, node->end))
    {
        if (strcmp(b->chr[node->begin], b->chr[node->end]) == 0)
        {
            double length = fabs(b->cm[node->end] - b->cm[node->begin]);
            add_edge(g, length, node->first, node->second);
        }
    }

    if (node->left != NULL && node->left->max >= qbegin)
    {
        return match_adjsearch(b, node->left, g, qbegin, qend);
    }

    return match_adjsearch(b, node->right, g, qbegin, qend);
}

int match_coasearch(pbwt_t *b, match_t *node, double **cmatrix, size_t qbegin, size_t qend)
{
    if (node == NULL)
    {
        return 0;
    }

    if (match_overlap(qbegin, qend, node->begin, node->end))
    {
        if (strcmp(b->chr[node->begin], b->chr[node->end]) == 0)
        {
            double length = fabs(b->cm[node->end] - b->cm[node->begin]);
            cmatrix[node->first][node->second] += length;
            cmatrix[node->second][node->first] += length;
        }
    }

    if (node->left != NULL && node->left->max >= qbegin)
    {
        return match_coasearch(b, node->left, cmatrix, qbegin, qend);
    }

    return match_coasearch(b, node->right, cmatrix, qbegin, qend);
}

int match_regsearch(pbwt_t *b, match_t *node, khash_t(floats) *result, size_t qbegin, size_t qend)
{
    if (node == NULL)
    {
        return 0;
    }

    if (match_overlap(qbegin, qend, node->begin, node->end))
    {
        if (strcmp(b->chr[node->begin], b->chr[node->end]) == 0)
        {
            int a = 0;
            khint_t k = 0;
            double length = fabs(b->cm[node->end] - b->cm[node->begin]);
            size_t qs = 0;

            qs = b->is_query[node->first] ? node->second : node->first;
            k = kh_put(floats, result, b->reg[qs], &a);
            if (a == 0)
            {
                double ent = kh_value(result, k);
                ent += length;
                kh_value(result, k) = ent;
            }
            else
            {
                kh_value(result, k) = length;
            }
        }
    }

    if (node->left != NULL && node->left->max >= qbegin)
    {
        return match_regsearch(b, node->left, result, qbegin, qend);
    }

    return match_regsearch(b, node->right, result, qbegin, qend);
}

size_t match_count(pbwt_t *b, match_t *node, double *al)
{
    static size_t count = 0;

    if (node == NULL)
    {
        return count;
    }

    match_count(b, node->left, al);
    *al += fabs(b->cm[node->end] - b->cm[node->begin]);
    count++;
    match_count(b, node->right, al);

    return count;
}

match_t *match_insert(match_t *node, const size_t first, const size_t second,
                      const size_t begin, const size_t end)
{
    /* Don't add if it is a duplicate entry */
    if (node && node->first == first && node->second == second &&
        node->begin == begin && node->end == end)
    {
        return node;
    }

    /* Add a leaf node */
    if (node == NULL)
    {
        return match_new(first, second, begin, end);
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

int match_overlap(const size_t begin1, const size_t end1, const size_t begin2, const size_t end2)
{
    return begin1 <= end2 && begin2 <= end1 ? 1 : 0;
}
