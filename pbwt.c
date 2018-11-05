#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pbwt.h"

pbwt_t *
pbwt_init (const size_t nsite, const size_t nsam)
{
	size_t i = 0;
	pbwt_t *b = NULL;

	b = (pbwt_t *)malloc(sizeof(struct pbwt));
	b->nsite = nsite;
	b->nsam = nsam;
	b->ppa = (size_t *)malloc(nsam * sizeof(size_t));
	b->div = (size_t *)malloc(nsam * sizeof(size_t));
	b->sid = (char **)malloc(nsam * sizeof(char *));
	b->reg = (char **)malloc(nsam * sizeof(char *));
	b->data = (unsigned char **)malloc(nsam * sizeof(unsigned char *));
	for (i = 0; i < nsam; ++i)
	{
		b->ppa[i] = i;
		b->div[i] = 0;
		b->data[i] = (unsigned char *)malloc(nsite * sizeof(unsigned char));
	}
	return b;
}

int
pbwt_destroy (pbwt_t *b)
{
	size_t i = 0;
	for (i=0; i < b->nsam; ++i)
	{
		free(b->sid[i]);
		free(b->reg[i]);
		free(b->data[i]);
	}
	free(b->sid);
	free(b->reg);
	free(b->data);
	free(b->div);
	free(b->ppa);
	free(b);
	return 0;
}

int
pbwt_print (const pbwt_t *b)
{
	size_t i = 0;
	size_t j = 0;
	for (i = 0; i < b->nsam; ++i)
	{
		size_t index = b->ppa[i];
		printf("%2zu | %5zu | ", b->div[i], index);
		for (j = 0; j < b->nsite; ++j)
			putchar((char)(b->data[index][j]));
		putchar('\n');
	}
	return 0;
}

int
build_prefix_array (pbwt_t *b)
{
	size_t i = 0;
	size_t j = 0;
	size_t k = 0;
	size_t ia = 0;
	size_t ib = 0;
	size_t da = 0;
	size_t db = 0;
	size_t *ara = malloc(b->nsam * sizeof(size_t));
	size_t *arb = malloc(b->nsam * sizeof(size_t));
	size_t *ard = malloc(b->nsam * sizeof(size_t));
	size_t *are = malloc(b->nsam * sizeof(size_t));

	for (i = 0; i < b->nsite; ++i)
	{
		ia = 0;
		ib = 0;
		da = i + 1;
		db = i + 1;

		for (j = 0; j < b->nsam; ++j)
		{
			size_t ix = b->ppa[j];
			size_t ms = b->div[j];

			if (ms > da)
				da = ms;
			if (ms > db)
				db = ms;

			if (b->data[ix][i] == '0')
			{
				ara[ia] = ix;
				ard[ia] = da;
				da = 0;
				++ia;
			}
			else
			{
				arb[ib] = ix;
				are[ib] = db;
				db = 0;
				++ib;
			}
		}

		/* Concatenate arrays */
		if (i < b->nsite - 1)
		{
			for (j=0; j < ia; ++j)
			{
				b->ppa[j] = ara[j];
				b->div[j] = ard[j];
			}
			for (j=0, k=ia; j < ib; ++j, ++k)
			{
				b->ppa[k] = arb[j];
				b->div[k] = are[j];
			}
		}
	}

	/* Free allocated memory */
	free(ara);
	free(arb);
	free(ard);
	free(are);

	return 0;
}

int report_set_maximal_matches(pbwt_t *b)
{
	return 0;
}

int report_long_matches(pbwt_t *b)
{
	return 0;
}
