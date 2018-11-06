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
	b->datasize = nsite * nsam;
	b->is_compress = 0;
	b->ppa = (size_t *)malloc(nsam * sizeof(size_t));
	b->div = (size_t *)malloc(nsam * sizeof(size_t));
	b->sid = (char **)malloc(nsam * sizeof(char *));
	b->reg = (char **)malloc(nsam * sizeof(char *));
	b->data = (unsigned char *)malloc(nsite * nsam * sizeof(unsigned char));
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

	for (i = 0; i < b->nsam; ++i)
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
pbwt_write (const char *outfile, pbwt_t *b)
{
	/* If data aren't compressed, then compress */
	if (b->is_compress == 0)
		pbwt_compress(b);

	size_t i = 0;
	FILE *fout;

	/* Open the binary output file stream */
	fout = fopen(outfile, "wb");

	/* Write the data to the output file */
	fwrite((const void *)&(b->nsite), sizeof(size_t), 1, fout);
	fwrite((const void *)&(b->nsam), sizeof(size_t), 1, fout);
	fwrite((const void *)&(b->datasize), sizeof(size_t), 1, fout);
	fwrite((const void *)b->data, sizeof(unsigned char), b->datasize, fout);
	fwrite((const void *)b->ppa, sizeof(size_t), b->nsam, fout);
	fwrite((const void *)b->div, sizeof(size_t), b->nsam, fout);

	/* Write sample info */
	for (i=0; i < b->nsam; i++)
	{
		size_t len = strlen(b->sid[i]);
		fwrite((const void *)&len, sizeof(size_t), 1, fout);
		fwrite(b->sid[i], sizeof(char), len, fout);
		len = strlen(b->reg[i]);
		fwrite((const void *)&len, sizeof(size_t), 1, fout);
		fwrite(b->reg[i], sizeof(char), len, fout);
	}

	/* Close output file stream */
	fclose(fout);

	return 0;
}

pbwt_t *
pbwt_read (const char *infile)
{
	size_t i = 0;
	size_t ret = 0;
	size_t nsite = 0;
	size_t nsam = 0;
	pbwt_t *b = NULL;
	FILE *fin;

	/* Open binary input file stream */
	fin = fopen(infile, "rb");

	/* Read the data into memory */

	/* Read metadata */
	ret = fread(&nsite, sizeof(size_t), 1, fin);
	ret = fread(&nsam, sizeof(size_t), 1, fin);

	/* Initialize the new pbwt */
	b = pbwt_init(nsite, nsam);

	/* Read haplotype and pbwt data */
	ret = fread(&(b->datasize), sizeof(size_t), 1, fin);
	ret = fread(b->data, sizeof(unsigned char), b->datasize, fin);
	ret = fread(b->ppa, sizeof(size_t), b->nsam, fin);
	ret = fread(b->div, sizeof(size_t), b->nsam, fin);

	/* Mark pbwt as compressed */
	b->is_compress = 1;

	/* Read sample info */
	for (i = 0; i < nsam; i++)
	{
		size_t len;
		ret = fread(&len, sizeof(size_t), 1, fin);
		b->sid[i] = (char *) malloc((len + 1) * sizeof(char));
		ret = fread(b->sid[i], sizeof(char), len, fin);
		b->sid[i][len] = '\0';
		ret = fread(&len, sizeof(size_t), 1, fin);
		b->reg[i] = (char *) malloc((len + 1) * sizeof(char));
		ret = fread(b->reg[i], sizeof(char), len, fin);
		b->reg[i][len] = '\0';
	}

	/* Close the input file stream */
	fclose(fin);

	return b;
}

int
pbwt_uncompress (pbwt_t *b)
{
	/* If data are already uncompressed */
	if (b->is_compress == 0)
		return 0;

	unsigned char *g = (unsigned char *)malloc(b->nsite * b->nsam * sizeof(unsigned char));
    z_stream infstream;

	/* Initialize inflate stream */
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
	infstream.avail_in = b->datasize;
    infstream.next_in = (Bytef *)b->data;
    infstream.avail_out = b->nsite * b->nsam;
    infstream.next_out = (Bytef *)g;

    /* Inflate the data */
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);

	/* Update pbwt data structure */
	free(b->data);
	b->data = g;
	b->datasize = infstream.total_out;
	b->is_compress = 0;

	return 0;
}

int
pbwt_compress (pbwt_t *b)
{
	/* Check if data are already compressed */
	if (b->is_compress)
		return 0;

	int ret;
	unsigned char *f = (unsigned char *) malloc (b->nsam * b->nsite * sizeof(unsigned char));
	z_stream defstream;

	/* Setup the deflate stream */
	defstream.zalloc = Z_NULL;
	defstream.zfree = Z_NULL;
	defstream.opaque = Z_NULL;
	defstream.avail_in = b->datasize;
	defstream.next_in = (Bytef *)b->data;
	defstream.avail_out = b->datasize;
	defstream.next_out = (Bytef *)f;

	/* Deflate the data */
	ret = deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
	if (ret != Z_OK)
	{
		fputs ("libpwbt [ERROR]: cannot initialize compression stream", stderr);
		exit (EXIT_FAILURE);
	}
	deflate(&defstream, Z_FINISH);
	deflateEnd(&defstream);

	/* Update pbwt data structure */
	free(b->data);
	b->is_compress = 1;
	b->data = f;
	b->datasize = defstream.total_out;

	return ret;
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
