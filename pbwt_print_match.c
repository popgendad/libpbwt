#include <stdio.h>
#include <stdlib.h>
#include "pbwt.h"

int
pbwt_print_match (match_t *list)
{
    if (list != NULL)
    {
        match_t *p;
        for (p = list; p != NULL; p = p->next)
            printf ("%zu\t%zu\t%zu\t%zu\n", p->first, p->second, p->begin, p->end);
    }

    return 0;
}
