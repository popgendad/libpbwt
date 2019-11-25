#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pbwt.h>


int
main(int argc, char *argv[])
{
    int v = 0;
    size_t qid = 0;
    size_t qstart = 0;
    size_t qend = 0;
    pbwt_t *b = NULL;
    double minlen = 0.5;
    char *infile = NULL;
    khint_t k;
    khash_t(floats) *result;

    result = kh_init(floats);
    v = 0;

    /* Define infile name */
    if (argv[1] != NULL)
        infile = strdup(argv[1]);
    else
    {
        fputs("Usage: ./ptest2 <pbwt file> <minlen> <hap_index> <start> <end>\n", stderr);
        return 1;
    }

    minlen = atof(argv[2]);
    qid = (size_t)(atoi(argv[3]));
    qstart = (size_t)(atoi(argv[4]));
    qend = (size_t)(atoi(argv[5]));

    /* Read in the pbwt file from disk */
    b = pbwt_read(infile);
    if (b == NULL)
    {
        fprintf(stderr, "Cannot read data from %s\n", infile);
        return 1;
    }

    pbwt_uncompress(b);

    /* Build the prefix and divergence arrays for the subset pbwt */
    v = pbwt_build(b);

    b->is_query[qid] = 1;

    /* Find all set-maximal matches */
    v = pbwt_query_match(b, minlen);

    match_search(b, b->match, result, qstart, qend);

    /* Print hash */
    size_t i = 0;
    size_t nregs = 0;
    char **reglist = NULL;
    reglist = pbwt_get_reglist(b, &nregs);

    for (i = 0; i < nregs; ++i)
    {
        k = kh_get(floats, result, reglist[i]);
        if (kh_exist(result, k) && k != kh_end(result))
            fprintf(stdout, "%s\t%s\t%s\t%1.5lf\n", infile, b->reg[qid], reglist[i], kh_value(result, k));
        else
            fprintf(stdout, "%s\t%s\t%s\t0.00000\n", infile, b->reg[qid], reglist[i]);
    }

    /* Free memory for the original pbwt data structure */
    pbwt_destroy(b);
    free(reglist);

    return v;
}

