#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pbwt.h>

int main (int argc, char *argv[])
{
    int v;
    pbwt_t *b;
    pbwt_t *s;
    char *infile;
    char *inpop;

    v = 0;

    /* Define infile name */
    if (argv[1] != NULL)
        infile = strdup (argv[1]);
    else
    {
        fputs ("Usage: ./ptest <pbwt file> <optional pop>\n", stderr);
        return 1;
    }

    if (argv[2] != NULL)
        inpop = strdup (argv[2]);
    else
        inpop = NULL;

    /* Read in the pbwt file from disk */
    b = pbwt_read (infile);
    if (b == NULL)
    {
        fprintf (stderr, "Cannot read data from %s\n", infile);
        return 1;
    }

    pbwt_uncompress (b);

    /* Subset the pbwt to the Beringia region */
    if (inpop != NULL)
    {
        s = pbwt_subset_with_query (b, inpop, 1000);
        if (s == NULL)
        {
            fputs ("Failed to subset the data", stderr);
            return -1;
        }

        /* Build the prefix and divergence arrays for the subset pbwt */
        v = pbwt_build (s);

        /* Print the pbwt */
        pbwt_print (s);

        /* Find all set-maximal matches */
        v = pbwt_set_match (s, 0.1);

        /* Print matches to stdout */
        match_print (s, s->match);
        v = match_search (s, s->match, 1, 10);

        double c;
        c = match_coverage (s, s->match);
        printf ("Coverage: %1.5lf\n", c);

        /* Free memory for the subset pbwt data structure */
        if (s != NULL)
            pbwt_destroy (s);
    }

    /* Free memory for the original pbwt data structure */
    pbwt_destroy (b);

    return v;
}

