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
	b->data = (unsigned char *)malloc(nsam * nsite * sizeof(unsigned char));
	for (i = 0; i < nsam; ++i)
	{
		b->ppa[i] = i;
		b->div[i] = 0;
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
pbwt_write (pbwt_t *b)
{
	return 0;
}

pbwt_t *
pbwt_read (const char *filename)
{
	pbwt_t *b;
	return b;
}

unsigned long int
pbwt_uncompress (pbwt_t *b, size_t defsize)
{
/* 	unsigned char *g = (unsigned char *)malloc(b->nsite * b->nsam * sizeof(unsigned char));
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    infstream.avail_in = (size_t)(defstream.next_out - f); // size of input
	infstream.avail_in = (size_t)(defsize - g);
    infstream.next_in = (Bytef *)f; // input char array
	infstream.next_in = (Bytef *)defsize;
    infstream.avail_out = b->nsite * b->nsam; // size of output
    infstream.next_out = (Bytef *)g; // output char array

    // the actual DE-compression work
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);

    printf("Uncompressed size is: %lu\n", strlen((char*)g));
	free(g); */
	return 0;
}

unsigned long int
pbwt_compress (pbwt_t *b)
{
	unsigned char *f = (unsigned char *)malloc(b->nsite * b->nsam * sizeof(unsigned char));
	z_stream defstream;
	defstream.zalloc = Z_NULL;
	defstream.zfree = Z_NULL;
	defstream.opaque = Z_NULL;
	defstream.avail_in = b->nsite * b->nsam;
	defstream.next_in = (Bytef *)b->data;
	defstream.avail_out = b->nsite * b->nsam;
	defstream.next_out = (Bytef *)f;

	deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
	deflate(&defstream, Z_FINISH);
	deflateEnd(&defstream);
	printf("nsites: %lu\n", b->nsite);
	printf("nsam: %lu\n", b->nsam);
	printf("Uncompressed size: %lu\n", defstream.total_in);
	printf("Compressed size: %lu\n", defstream.total_out);
	printf("Compression ratio: %lf\n", (double)(defstream.total_in)/(double)(defstream.total_out));
	free(f);
	return defstream.total_out;
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
			putchar((char)(b->data[TWODCORD(index, b->nsite, j)]));
		printf(" %s  %s\n", b->sid[index], b->reg[index]);
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

			if (b->data[TWODCORD(ix, b->nsite, i)] == '0')
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
