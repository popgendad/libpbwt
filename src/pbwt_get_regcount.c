#include <stdlib.h>
#include <stdint.h>
#include "pbwt.h"

khash_t(integer) *pbwt_get_regcount(pbwt_t *b)
{
	if (b == NULL)
	{
		return NULL;
	}

    int absent = 0;
    size_t i = 0;
	khint_t it = 0;
	khash_t(integer) *regcount = NULL;

	regcount = kh_init(integer);

    for (i = 0; i < b->nsam; ++i)
    {
    	it = kh_put(integer, regcount, b->reg[i], &absent);
	    if (absent)
	    {
	        kh_key(regcount, it) = strdup(b->reg[i]);
	        kh_value(regcount, it) = 0;
	    }
	    else
	    {
	    	uint64_t count = kh_value(regcount, it);
	        kh_value(regcount, it) = count + 1;
	    }
	}

	return regcount;
}