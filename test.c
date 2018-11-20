#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pbwt.h>

int main (int argc, char *argv[])
{
    int v;
    size_t i;
    pbwt_t *b;
    pbwt_t *s;
    match_t *x;
    char *infile;

    /* Define infile name */
    infile = strdup (argv[1]);

    /* Read in the pbwt file from disk */
    b = pbwt_read (infile);

    v = pbwt_uncompress (b);

    /* Subset the pbwt to the Beringia region */
    s = pbwt_subset (b, "Beringia");
    if (s == NULL)
        return -1;

    /* Build the prefix and divergence arrays for the subset pbwt */
    v = pbwt_build (s);

    /* Print the pbwt */
    /*pbwt_print (s);*/

    /* Find all set-maximal matches */
    x = pbwt_match (s, 0, 0.5);

    /* Print matches to stdout */
    pbwt_print_match (s, x);

    /* Free memory for the original pbwt data structure */
    pbwt_destroy (b);
    pbwt_destroy (s);

    return 0;
}

