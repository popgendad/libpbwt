#include <stdio.h>
#include <stdlib.h>
#include "pbwt.h"

match_t *
match_new (const size_t first, const size_t second, const size_t begin, const size_t end)
{
    match_t *node;

    node = (match_t *) malloc (sizeof(match_t));
    if (node == NULL)
    {
        perror ("libpbwt [ERROR]");
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

int
match_search (pbwt_t *b, match_t *node, size_t qbegin, size_t qend)
{
    if (node == NULL)
        return 0;
    if (match_overlap (qbegin, qend, node->begin, node->end))
    {
        if (b->is_query[node->first])
            printf ("%s\t%s\t%s\t%s\t%1.5lf\n", b->sid[node->first], b->reg[node->first],
                 b->sid[node->second], b->reg[node->second], b->cm[node->end] - b->cm[node->begin]);
        else
            printf ("%s\t%s\t%s\t%s\t%1.5lf\n", b->sid[node->second], b->reg[node->second],
                b->sid[node->first], b->reg[node->first], b->cm[node->end] - b->cm[node->begin]);
    }
    if (node->left != NULL && node->left->max >= qbegin)
    {
        return match_search (b, node->left, qbegin, qend);
    }
    return match_search (b, node->right, qbegin, qend);
}

size_t
match_count (pbwt_t *b, match_t *node, double *al)
{
    static size_t count = 0;
    if (node == NULL)
        return count;
    match_count (b, node->left, al);
    *al += b->cm[node->end] - b->cm[node->begin];
    count++;
    match_count (b, node->right, al);
    return count;
}

double
match_coverage (pbwt_t *b, match_t *node)
{
    double total_length;
    double avg_length;
    size_t num_matches;

    num_matches = 0;
    avg_length = 0.0;
    total_length = b->cm[b->nsite-1] - b->cm[0];
    num_matches = match_count (b, node, &avg_length);
    if (num_matches == 0)
        return 0.0;
    avg_length /= (double)(num_matches);
    return ((2.0 * num_matches) / (double)(b->nsam * (b->nsam-1))) * (avg_length / total_length);
}

double
match_query_coverage (pbwt_t *b, match_t *node)
{
    double total_length;
    double avg_length;
    size_t num_matches;

    num_matches = 0;
    avg_length = 0.0;
    total_length = b->cm[b->nsite-1] - b->cm[0];
    num_matches = match_count (b, node, &avg_length);
    if (num_matches == 0)
        return 0.0;
    avg_length /= (double)(num_matches);
    return (num_matches / (double)(b->nsam - 1)) * (avg_length / total_length);
}

match_t *
match_insert (match_t *node, const size_t first, const size_t second,
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
        return match_new (first, second, begin, end);
    }

    /* Traverse the tree */
    if (begin < node->begin)
        node->left = match_insert (node->left, first, second, begin, end);
    else
        node->right = match_insert (node->right, first, second, begin, end);

    if (node->max < end)
        node->max = end;

    return node;
}

int
match_overlap (const size_t begin1, const size_t end1, const size_t begin2, const size_t end2)
{
    if (begin1 <= end2 && begin2 <= end1)
        return 1;
    else
        return 0;
}
