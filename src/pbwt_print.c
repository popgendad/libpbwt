#include <stdio.h>
#include "pbwt.h"

int
pbwt_print(const pbwt_t *b)
{
    /* Check if pointer is NULL */
    if (b == NULL)
    {
        return -1;
    }

    size_t i = 0;
    size_t j = 0;

    for (i = 0; i < b->nsam; ++i)
    {
        size_t index = 0;

        index = b->ppa[i];

        /* Print prefix and divergence array member for haplotype i */
        printf("%5zu\t%5zu\t", b->div[i], index);

        /* Print binary haplotype array for haplotype i */
        for (j = 0; j < b->nsite; ++j)
        {
            putchar((char)(b->data[TWODCORD(index, b->nsite, j)]));
        }

        /* Print sample identifier associated with haplotype i */
        if (b->sid[index])
        {
            printf("\t%s", b->sid[index]);
        }
        else
        {
            fprintf(stderr, "libpbwt [ERROR]: problem reading sample identifier with index %5zu\n", index);
            return -1;
        }

        /* If a region is present */
        if (b->reg[index])
        {
            printf("\t%s\n", b->reg[index]);
        }
        else
        {
            putchar('\n');
        }
    }

    return 0;
}
