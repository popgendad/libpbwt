#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pbwt.h>

int
main (int argc, char *argv[])
{
    int v;
    size_t qid;
    pbwt_t *b;
    pbwt_t *s;
    pbwt_t *p;
    const double minlen = 0.4;
    char *infile;

    v = 0;

    /* Define infile name */
    if (argv[1] != NULL)
        infile = strdup (argv[1]);
    else
    {
        fputs ("Usage: ./ptest <pbwt file> <query id>\n", stderr);
        return 1;
    }

    if (argv[2] != NULL)
        qid = atol (argv[2]);
    else
    {
        fputs ("Usage: ./ptest <pbwt file> <query id>\n", stderr);
        return 1;
    }

    /* Read in the pbwt file from disk */
    b = pbwt_read (infile);
    if (b == NULL)
    {
        fprintf (stderr, "Cannot read data from %s\n", infile);
        return 1;
    }

    pbwt_uncompress (b);

    size_t i;
    size_t nregs;
    char **reglist;
    reglist = pbwt_get_reglist (b, &nregs);
    printf ("Query: %s    %s\n", b->sid[qid], b->reg[qid]);
    for (i = 0; i < nregs; ++i)
    {
        p = pbwt_subset (b, reglist[i]);
        s = pbwt_subset_with_query (b, reglist[i], qid);
        if (s == NULL)
        {
            fputs ("Failed to subset the data", stderr);
            return -1;
        }

        /* Build the prefix and divergence arrays for the subset pbwt */
        v = pbwt_build (s);
        v = pbwt_build (p);

        /* Find all set-maximal matches */
        v = pbwt_query_match (s, minlen);
        v = pbwt_set_match (p, minlen);

        double c;
        double e;
        c = match_query_coverage (s, s->match);
        e = match_coverage (p, p->match);
        printf ("%s\t%1.5e\t%1.5e\t%1.5lf\n", reglist[i], c, e, c / e);

        /* Free memory for the subset pbwt data structure */
        if (s != NULL)
            pbwt_destroy (s);
        if (p != NULL)
            pbwt_destroy (p);
    }

    /* Free memory for the original pbwt data structure */
    pbwt_destroy (b);

    return v;
}

