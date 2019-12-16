#include <string.h>
#include "pbwt.h"

size_t remove_dups(size_t, char **);
int cmp_strings(const void *, const void *);

char **pbwt_get_reglist(pbwt_t *b, size_t *nr)
{
    size_t i = 0;
    size_t r = 0;
    char **array = NULL;

    /* Allocate heap memory for reglist array */
    array = (char **)malloc(b->nsam * sizeof(char *));
    if (array == NULL)
    {
        return NULL;
    }

    /* Copy sample regions to new array */
    for (i = 0; i < b->nsam; ++i)
    {
        array[i] = strdup(b->reg[i]);
    }

    /* Sort the new array */
    qsort(array, b->nsam, sizeof(char *), cmp_strings);

    /* Remove duplicate region strings from new array */
    r = remove_dups(b->nsam, array);
    *nr = r;

    return array;
}

int cmp_strings(const void *a, const void *b)
{
    return strcmp(*(const char**)a, *(const char**)b);
}

size_t remove_dups(size_t n, char **array)
{
    size_t i = 0;
    size_t j = 1;
    size_t k = 1;

    for (i = 0; i < n; ++i)
    {
        for (j = i + 1, k = j; j < n; ++j)
        {
            if (strcmp(array[i], array[j]))
            {
                array[k++] = array[j];
            }
        }
        n -= j - k;
    }

    return n;
}
