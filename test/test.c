#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pbwt.h>

int
main (int argc, char *argv[])
{
    int v;
    pbwt_t *b;
    pbwt_t *s;
    const double minlen = 0.4;
    char *infile;

    v = 0;

    /* Define infile name */
    if (argv[1] != NULL)
        infile = strdup (argv[1]);
    else
    {
        fputs ("Usage: ./ptest <pbwt file> <optional pop>\n", stderr);
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
    for (i = 0; i < nregs; ++i)
    {
        s = pbwt_subset (b, reglist[i]);
        if (s == NULL)
        {
            fputs ("Failed to subset the data", stderr);
            return -1;
        }

        /* Build the prefix and divergence arrays for the subset pbwt */
        v = pbwt_build (s);

        /* Find all set-maximal matches */
        v = pbwt_set_match (s, minlen);

        double c;
        c = match_coverage (s, s->match);
        printf ("%s\t%1.5e\n", reglist[i], c);

        /* Free memory for the subset pbwt data structure */
        if (s != NULL)
            pbwt_destroy (s);
    }

    /* Free memory for the original pbwt data structure */
    pbwt_destroy (b);

    return v;
}

