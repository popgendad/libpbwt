
#include <pbwt.h>

int main(int argc, char *argv[])
{
    int v = 0;
    size_t i = 0;
    size_t nsites = 0;
    pbwt_t *b = NULL;

    const char *jj = pbwt_version();
    printf("version: %s\n", jj);

    /* Read in the pbwt file from disk */
    b = pbwt_read(argv[1]);
    if (b == NULL)
    {
        fprintf(stderr, "Cannot read data from %s\n", argv[1]);
        return 1;
    }

	/* Unless specified otherwise, indicate penultimate site is end site */
    nsites = b->nsite;

    /* Uncompress the haplotype data */
    pbwt_uncompress(b);

    /* Query or coancestry matrix */
    double **cmatrix = NULL;
    size_t j = 0;

    cmatrix = (double **)malloc(b->nsam * sizeof(double *));
    for (i = 0; i < b->nsam; ++i)
    {
        cmatrix[i] = (double *)malloc(b->nsam * sizeof(double));
        for (j = 0; j < b->nsam; ++j)
        {
            cmatrix[i][j] = 0.0;
        }
    }

    v = pbwt_set_match(b, 0.5);
    match_search(b, b->match, cmatrix, 0, nsites-1);
    match_print(b, b->match);

	return 0;
}
