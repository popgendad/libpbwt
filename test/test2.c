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
        fputs ("Usage: ./ptest <pbwt file>\n", stderr);
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
    p = pbwt_subset (b, "Beringia");

    /* Build the prefix and divergence arrays for the subset pbwt */
    v = pbwt_build (p);

    /* Find all set-maximal matches */
    /*v = pbwt_find_match (p, 5);*/
    v = pbwt_set_match(p, 0.05);

    pbwt_print(p);
    match_print(p, p->match);

    if (p != NULL)
        pbwt_destroy (p);

    /* Free memory for the original pbwt data structure */
    pbwt_destroy (b);

    return v;
}

