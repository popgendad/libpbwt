#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pbwt.h>


void add_region(pbwt_t *b, const size_t first, const size_t second, const size_t begin, const size_t end)
{
    if (b->reghash == NULL)
    {
        b->reghash = kh_init(floats);
    }
    int a = 0;
    size_t qs = 0;
    khint_t k = 0;
    double length = b->cm[end] - b->cm[begin];
    qs = b->is_query[first] ? second : first;
    k = kh_put(floats, b->reghash, b->reg[qs], &a);
    if (a == 0)
    {
        double ent = kh_value(b->reghash, k);
        ent += length;
        kh_value(b->reghash, k) = ent;
    }
    else
    {
        kh_value(b->reghash, k) = length;
    }
}

int main(int argc, char *argv[])
{
    int v = 0;
    size_t qid = 0;
    pbwt_t *b = NULL;
    double minlen = 0.5;
    char *infile = NULL;
    khint_t k = 0;


    /* Define infile name */
    if (argv[1] != NULL)
        infile = strdup(argv[1]);
    else
    {
        fputs("Usage: ./ptest2 <pbwt file> <minlen> <hap_index>\n", stderr);
        return 1;
    }

    minlen = atof(argv[2]);
    qid = (size_t)(atoi(argv[3]));

    /* Read in the pbwt file from disk */
    b = pbwt_read(infile);
    if (b == NULL)
    {
        fprintf(stderr, "Cannot read data from %s\n", infile);
        return 1;
    }

    pbwt_uncompress(b);
    b->is_query[qid] = 1;

    /* Find all set-maximal matches */
    v = pbwt_all_query_match(b, minlen, add_region);

    /* Print hash */
    size_t i = 0;
    size_t nregs = 0;
    char **reglist = NULL;
    reglist = pbwt_get_reglist(b, &nregs);

    for (i = 0; i < nregs; ++i)
    {
        k = kh_get(floats, b->reghash, reglist[i]);
        if (kh_exist(b->reghash, k) && k != kh_end(b->reghash))
            fprintf(stdout, "%s\t%s\t%s\t%1.5lf\n", infile, b->reg[qid], reglist[i], kh_value(b->reghash, k));
        else
            fprintf(stdout, "%s\t%s\t%s\t0.00000\n", infile, b->reg[qid], reglist[i]);
    }

    /* Free memory for the original pbwt data structure */
    pbwt_destroy(b);
    free(reglist);

    return v;
}

