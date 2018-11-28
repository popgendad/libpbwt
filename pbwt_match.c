#include <stdio.h>
#include "pbwt.h"

match_t * match_insert (match_t *, const size_t, const size_t, const size_t, const size_t);
size_t match_count (pbwt_t *, match_t *, double *);
match_t * match_new (const size_t, const size_t, const size_t, const size_t);
int match_overlap (const size_t, const size_t, const size_t, const size_t);

int
pbwt_match (pbwt_t *b, const size_t query_index, const double minlen)
{
    size_t i;
    size_t j;
    size_t k;
    size_t *sdiv;
    size_t *jppa;
    match_t *intree;

    intree = NULL;

    /* Allocate heap memory for prefix and divergence arrays */
    sdiv = (size_t *) malloc ((b->nsam + 1) * sizeof(size_t));
    if (sdiv == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }
    jppa = (size_t *) malloc (b->nsam * sizeof(size_t));
    if (jppa == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }

    /* Initialize prefix and divergence arrays */
    for (i = 0; i < b->nsam; ++i)
    {
        sdiv[i] = 0;
        jppa[i] = i;
    }

    for (i = 0; i < b->nsite; ++i)
    {
        sdiv[0] = i + 1;
        sdiv[b->nsam] = i + 1;

        for (j = 0; j < b->nsam; ++j)
        {
            int m = j - 1;
            int n = j + 1;
            int mc = 0;

            if (sdiv[j] <= sdiv[j+1])
            {
                while (sdiv[m+1] <= sdiv[j])
                {
                    if (b->data[TWODCORD(jppa[m], b->nsite, i)] ==
                        b->data[TWODCORD(jppa[j], b->nsite, i)])
                    {
                        mc = 1;
                        break;
                    }
                    m -= 1;
                }
            }

            if (mc)
                continue;

            if (sdiv[j] >= sdiv[j+1])
            {
                while (sdiv[n] <= sdiv[j+1])
                {
                    if (b->data[TWODCORD(jppa[n], b->nsite, i)] ==
                        b->data[TWODCORD(jppa[j], b->nsite, i)])
                    {
                        mc = 1;
                        break;
                    }
                    n += 1;
                }
            }

            if (mc)
                continue;

            for (k = m + 1; k < j; ++k)
            {
                double match_dist = (b->cm[i] - b->cm[sdiv[j]]) / (b->cm[b->nsite-1] - b->cm[0]);
                if (sdiv[j] < i && match_dist >= minlen)
                {
                    if (jppa[j] < jppa[k])
                        intree = match_insert (intree, jppa[j], jppa[k], sdiv[j], i);
                    else
                        intree = match_insert (intree, jppa[k], jppa[j], sdiv[j], i);
                }
            }

            for (k = j + 1; k < n; ++k)
            {
                double match_dist = (b->cm[i] - b->cm[sdiv[j+1]]) / (b->cm[b->nsite-1] - b->cm[0]);
                if (sdiv[j+1] < i && match_dist >= minlen)
                {
                    if (jppa[j] < jppa[k])
                        intree = match_insert (intree, jppa[j], jppa[k], sdiv[j+1], i);
                    else
                        intree = match_insert (intree, jppa[k], jppa[j], sdiv[j+1], i);
                }
            }
        }

        if (i < b->nsite - 1)
        {
            size_t da;
            size_t db;
            size_t ia;
            size_t ib;
            size_t *ara;
            size_t *arb;
            size_t *ard;
            size_t *are;

            ara = (size_t *) malloc (b->nsam * sizeof(size_t));
            arb = (size_t *) malloc (b->nsam * sizeof(size_t));
            ard = (size_t *) malloc (b->nsam * sizeof(size_t));
            are = (size_t *) malloc (b->nsam * sizeof(size_t));

            da = i + 1;
            db = i + 1;
            ia = 0;
            ib = 0;

            for (j = 0; j < b->nsam; ++j)
            {
                size_t ix;
                size_t ms;

                ix = jppa[j];
                ms = sdiv[j];
                if (ms > da)
                    da = ms;
                if (ms > db)
                    db = ms;

                if (b->data[TWODCORD(ix, b->nsite, i)] == '0')
                {
                    ara[ia] = ix;
                    ard[ia] = da;
                    da = 0;
                    ++ia;
                }
                else
                {
                    arb[ib] = ix;
                    are[ib] = db;
                    db = 0;
                    ++ib;
                }
            }

            /* Concatenate arrays */
            for (j = 0; j < ia; ++j)
            {
                jppa[j] = ara[j];
                sdiv[j] = ard[j];
            }

            for (j = 0, k = ia; j < ib; ++j, ++k)
            {
                jppa[k] = arb[j];
                sdiv[k] = are[j];
            }

            free (ara);
            free (arb);
            free (ard);
            free (are);
        }
    }

    free (sdiv);
    free (jppa);

    b->match = intree;

    return 0;
}

int
pbwt_longest_match (pbwt_t *b, const size_t query_index, const double minlen)
{
    return 0;
}

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
        printf ("%s\t%s\t%s\t%s\t%1.5lf\n", b->sid[node->first], b->reg[node->first],
                b->sid[node->second], b->reg[node->second], b->cm[node->end] - b->cm[node->begin]);
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
    avg_length /= (double)(num_matches);
    return ((2.0 * num_matches) / (double)(b->nsam * b->nsam - 1)) * (avg_length / total_length);
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
