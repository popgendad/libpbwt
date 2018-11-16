#include <stdio.h>
#include <stdlib.h>
#include "pbwt.h"

int
pbwt_print_match (pbwt_t *b, match_t *list)
{
    if (list != NULL)
    {
        match_t *p;
        for (p = list; p != NULL; p = p->next)
            printf ("%s\t%s\t%zu\t%zu\n",
                    b->sid[p->first], b->sid[p->second], p->begin, p->end);
    }

    return 0;
}
