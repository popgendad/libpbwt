#include <stdlib.h>
#include <string.h>
#include "pbwt.h"

khash_t(string) *pbwt_get_sampdict(pbwt_t *b)
{
	if (b == NULL)
	{
		return NULL;
	}

    int absent = 0;
    size_t i = 0;
	khint_t it = 0;
	khash_t(string) *sampdict = NULL;

	sampdict = kh_init(string);

    for (i = 0; i < b->nsam; ++i)
    {
    	it = kh_put(string, sampdict, b->sid[i], &absent);
	    if (absent)
	    {
	        kh_key(sampdict, it) = strdup(b->sid[i]);
	    }
        kh_value(sampdict, it) = strdup(b->reg[i]);;
	}

	return sampdict;
}
