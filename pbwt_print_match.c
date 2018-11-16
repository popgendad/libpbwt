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
            printf ("%s\t%s\t%zu\t%zu\t%1.5lf\t%1.5lf\n",
                    b->sid[p->first], b->sid[p->second], p->begin, p->end, b->cm[p->begin], b->cm[p->end]);
    }

    return 0;
}
