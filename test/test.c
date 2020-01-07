#include <pbwt.h>

int main(int argc, char *argv[])
{
	int v = 0;
    double **r = NULL;
    pbwt_t *b = NULL;

    /* Read in the pbwt file from disk */
    b = pbwt_read(argv[1]);
    if (b == NULL)
    {
        fprintf(stderr, "Cannot read data from %s\n", argv[1]);
        return 1;
    }

    /* Uncompress the haplotype data */
    pbwt_uncompress(b);
	v = pbwt_build(b);

    r = pbwt_find_match(b, 0.5);
    size_t i, j;
    for (i = 0; i < b->nsam; ++i)
    {
        printf("%s\t%s", b->sid[i], b->reg[i]);
        for (j = 0; j < b->nsam; ++j)
        {
            printf("\t%1.4lf", r[i][j]);
        }
        putchar('\n');
    }

	return 0;
}
