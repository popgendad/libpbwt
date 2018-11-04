#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pbwt
{
	size_t nsite;
	size_t nsam;
	size_t *ppa;
	size_t *div;
};

const unsigned char X[8][6] = {
  {'0', '1', '0', '1', '0', '1'},
  {'1', '1', '0', '0', '0', '1'},
  {'1', '1', '1', '1', '1', '1'},
  {'0', '1', '1', '1', '1', '0'},
  {'0', '0', '0', '0', '0', '0'},
  {'1', '0', '0', '0', '1', '0'},
  {'1', '1', '0', '0', '0', '1'},
  {'0', '1', '0', '1', '1', '0'}
};

struct pbwt *
pbwt_init (const size_t nsite, const size_t nsam)
{
	size_t i = 0;
	struct pbwt *b;
	b = malloc(sizeof(struct pbwt));
	b->nsite = nsite;
	b->nsam = nsam;
	b->ppa = malloc(nsam * sizeof(size_t));
	b->div = malloc(nsam * sizeof(size_t));
	for (i = 0; i < nsam; ++i)
	{
		b->ppa[i] = i;
		b->div[i] = 0;
	}
	return b;
}

int
pbwt_destroy (struct pbwt *b)
{
	free(b->div);
	free(b->ppa);
	free(b);
	return 0;
}

int
pbwt_print (const struct pbwt *b)
{
	size_t i = 0;
	size_t j = 0;
	for (i = 0; i < b->nsam; ++i)
	{
		printf("%zu | %zu | ", b->div[i], b->ppa[i]);
		for (j = 0; j < b->nsite - 1; ++j)
			putchar((char)(X[i][j]));
		printf(" %c\n", (char)(X[i][b->nsite -1]));
	}
	return 0;
}

int
main (int argc, char **argv)
{
	size_t i = 0;
	size_t j = 0;
	size_t k = 0;
	size_t ia = 0;
	size_t ib = 0;
	size_t da = 0;
	size_t db = 0;
	struct pbwt *b;

	b = pbwt_init(6, 8);

	for (i = 0; i < b->nsite - 1; ++i)
	{
		ia = 0;
		ib = 0;
		da = i + 1;
		db = i + 1;
		size_t *ara = malloc(b->nsam * sizeof(size_t));
		size_t *arb = malloc(b->nsam * sizeof(size_t));
		size_t *ard = malloc(b->nsam * sizeof(size_t));
		size_t *are = malloc(b->nsam * sizeof(size_t));
		size_t *ma = malloc(b->nsam * sizeof(size_t));
		size_t *mb = malloc(b->nsam * sizeof(size_t));

		for (j = 0; j < b->nsam; ++j)
		{
			size_t ix = b->ppa[j];
			size_t ms = b->div[j];

			if (ms > da)
				da = ms;
			if (ms > db)
				db = ms;

			if (X[ix][i] == '0')
			{
				ara[ia] = ix;
				ard[ia] = da;
				ma[ia] = ix;
				da = 0;
				++ia;
			}
			else
			{
				arb[ib] = ix;
				are[ib] = db;
				mb[ib] = ix;
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

		free(ara);
		free(arb);
		free(ard);
		free(are);
		free(ma);
		free(mb);
	}

	/* Print data structure */
	pbwt_print(b);

	pbwt_destroy(b);

	return 0;
}
