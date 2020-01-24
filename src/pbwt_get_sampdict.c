#include "pbwt.h"

khash_t(integer) *pbwt_get_sampdict(const pbwt_t *b)
{
	if (b == NULL)
	{
		return NULL;
	}

    int absent = 0;
    size_t i = 0;
	khint_t it = 0;
	khash_t(integer) *sampdict = NULL;

	sampdict = kh_init(integer);

    for (i = 0; i < b->nsam; ++i)
    {
    	it = kh_put(integer, sampdict, b->sid[i], &absent);
	    if (absent)
	    {
	        kh_key(sampdict, it) = strdup(b->sid[i]);
	    }
        kh_value(sampdict, it) = i;
	}

	return sampdict;
}
