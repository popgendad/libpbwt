#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "pbwt.h"

/* Declarations of non-API functions */
static void io_error (FILE *);
static int add_match (match_t *, size_t, size_t, size_t, size_t);

/* Definitions of API functions */

pbwt_t *
pbwt_init (const size_t nsite, const size_t nsam)
{
	size_t i;
	pbwt_t *b;

	/* Allocate heap memory for the pwbt data structure */

	b = (pbwt_t *) malloc (sizeof(pbwt_t));
	if (!b)
	{
		perror ("libpbwt [ERROR]");
		return NULL;
	}

	b->nsite = nsite;
	b->nsam = nsam;
	b->datasize = nsite * nsam;
	b->is_compress = 0;
	b->match = NULL;

	b->ppa = (size_t *) malloc (nsam * sizeof(size_t));
	if (!b->ppa)
	{
		perror ("libpbwt [ERROR]");
		pbwt_destroy (b);
		return NULL;
	}

	b->div = (size_t *) malloc (nsam * sizeof(size_t));
	if (!b->div)
	{
		perror ("libpbwt [ERROR]");
		pbwt_destroy (b);
		return NULL;
	}

	b->sid = (char **) malloc (nsam * sizeof(char *));
	if (!b->sid)
	{
		perror ("libpbwt [ERROR]");
		pbwt_destroy (b);
		return NULL;
	}

	b->reg = (char **) malloc (nsam * sizeof(char *));
	if (!b->reg)
	{
		perror ("libpbwt [ERROR]");
		pbwt_destroy (b);
		return NULL;
	}

	b->data = (unsigned char *) malloc (nsite * nsam * sizeof(unsigned char));
	if (!b->data)
	{
		perror ("libpbwt [ERROR]");
		pbwt_destroy (b);
		return NULL;
	}

	/* Initialize values for prefix and divergence arrays */
	for (i = 0; i < nsam; ++i)
	{
		b->ppa[i] = i;
		b->div[i] = 0;
	}

	return b;
}

void
pbwt_destroy (pbwt_t *b)
{
	/* Deallocate all heap memory pointed to by pbwt structure */
	if (b)
	{
		size_t i;
		if (b->sid)
		{
			for (i = 0; i < b->nsam; ++i)
				if (b->sid[i])
					free (b->sid[i]);
			free (b->sid);
		}
		if (b->reg)
		{
			for (i = 0; i < b->nsam; ++i)
				if (b->reg[i])
					free (b->reg[i]);
			free (b->reg);
		}
		if (b->data)
			free (b->data);
		if (b->div)
			free (b->div);
		if (b->ppa)
			free (b->ppa);
		if (b->match)
		{
			match_t *p;
			match_t *pn;
			p = b->match;
			while (p != NULL)
			{
				pn = b->match->next;
				free (p);
				p = pn;
			}
		}
		free (b);
	}
	return;
}

/* What to do with matching? */
int
pbwt_add (pbwt_t *b, const char *new_sid, const char *new_reg, const char *h1, const char *h2)
{
	int has_reg;
	size_t i;
	size_t len;
	size_t new_index1;
	size_t new_index2;

	/* Determine if a sample region is provided */
	if (new_reg)
		has_reg = 1;
	else
		has_reg = 0;

	/* Check length of input haplotype sequences */
	if (strlen (h1) != b->nsite || strlen (h2) != b->nsite)
	{
		fputs ("libpbwt [ERROR]: pbwt_add(), input haplotype is wrong length", stderr);
		return -1;
	}

	/* Check that new sample identifier string is not NULL */
	if (!new_sid)
	{
		fputs ("libpbwt [ERROR]: pbwt_add(), input sample identifier is NULL", stderr);
		return -1;
	}

	/* Assign the index positions of the new haplotypes */
	new_index1 = b->nsam;
	new_index2 = b->nsam + 1;

	/* Assign new haplotype sample size */
	b->nsam += 2;

	/* Grow the sample identifier list */
	b->sid = (char **) realloc (b->sid, b->nsam * sizeof(char *));

	/* Add new sample identifier to list */
	len = strlen (new_sid);
	b->sid[new_index1] = (char *) malloc (len * sizeof(char));
	b->sid[new_index2] = (char *) malloc (len * sizeof(char));
	strcpy (b->sid[new_index1], new_sid);
	strcpy (b->sid[new_index2], new_sid);

	/* Grow the region identifier list */
	b->reg = (char **) realloc (b->reg, b->nsam * sizeof(char *));

	/* Add new region identifier to list */
	if (has_reg)
	{
		len = strlen (new_reg);
		b->reg[new_index1] = (char *) malloc (len * sizeof(char));
		b->reg[new_index2] = (char *) malloc (len * sizeof(char));
		strcpy (b->reg[new_index1], new_reg);
		strcpy (b->reg[new_index2], new_reg);	
	}

	/* Re-calculate datasize */
	b->datasize = b->nsam * b->nsite;

	/* Grow the binary haplotype matrix */
	b->data = (unsigned char *) realloc (b->data, b->datasize * sizeof(unsigned char));

	/* Copy new binary haplotype data */
	memcpy (&(b->data[TWODCORD(new_index1, b->nsite, 0)]), h1, b->nsite);
	memcpy (&(b->data[TWODCORD(new_index2, b->nsite, 0)]), h2, b->nsite);

	/* Grow the prefix array */
	b->ppa = (size_t *) realloc (b->ppa, b->nsam * sizeof(size_t));

	/* Grow the divergence array */
	b->div = (size_t *) realloc (b->div, b->nsam * sizeof(size_t));

	/* Re-initialize prefix and divergence arrays */
	for (i = 0; i < b->nsam; ++i)
	{
		b->ppa[i] = i;
		b->div[i] = 0;
	}

	return 0;
}

int
pbwt_write (const char *outfile, pbwt_t *b)
{
	/* If data aren't compressed, then compress */
	if (!b->is_compress)
		pbwt_compress (b);

	size_t i;
	FILE *fout;

	/* Open the binary output file stream */
	fout = fopen (outfile, "wb");
	if (!fout)
	{
		perror ("libpbwt [ERROR]");
		return -1;
	}

	/* Write the data to the output file */

	/* Write the number of sites */
	if (fwrite ((const void *)&(b->nsite), sizeof(size_t), 1, fout) != 1)
	{
		io_error (fout);
		return -1;
	}

	/* Write the number of samples */
	if (fwrite ((const void *)&(b->nsam), sizeof(size_t), 1, fout) != 1)
	{
		io_error (fout);
		return -1;	
	}

	/* Write the size of the haplotype data */
	if (fwrite ((const void *)&(b->datasize), sizeof(size_t), 1, fout) != 1)
	{
		io_error (fout);
		return -1;
	}

	/* Write the haplotype data */
	if (fwrite ((const void *)b->data, sizeof(unsigned char), b->datasize, fout) != b->datasize)
	{
		io_error (fout);
		return -1;
	}

	/* Write the prefix array */
	if (fwrite ((const void *)b->ppa, sizeof(size_t), b->nsam, fout) != b->nsam)
	{
		io_error (fout);
		return -1;
	}

	/* Write the divergence array */
	if (fwrite ((const void *)b->div, sizeof(size_t), b->nsam, fout) != b->nsam)
	{
		io_error (fout);
		return -1;
	}

	/* Write sample info */
	for (i = 0; i < b->nsam; i++)
	{
		size_t len;

		/* Write the size of the sample identifier string */
		len = strlen (b->sid[i]);
		if (fwrite ((const void *)&len, sizeof(size_t), 1, fout) != 1)
		{
			io_error (fout);
			return -1;
		}

		/* Write the sample identifier string */
		if (fwrite (b->sid[i], sizeof(char), len, fout) != len)
		{
			io_error (fout);
			return -1;
		}

		/* Write the length of the region string */
		len = strlen (b->reg[i]);
		if (fwrite ((const void *)&len, sizeof(size_t), 1, fout) != 1)
		{
			io_error (fout);
			return -1;
		}

		/* Write the region string */
		if (fwrite (b->reg[i], sizeof(char), len, fout) != len)
		{
			io_error (fout);
			return -1;
		}
	}

	/* Close output file stream */
	fclose (fout);

	return 0;
}

pbwt_t *
pbwt_read (const char *infile)
{
	size_t i;
	size_t nsite;
	size_t nsam;
	pbwt_t *b;
	FILE *fin;

	/* Open binary input file stream */
	fin = fopen (infile, "rb");
	if (!fin)
	{
		perror ("libpbwt [ERROR]");
		return 0;
	}

	/* Read the data into memory */
	/* First read the number of sites */
	if (fread (&nsite, sizeof(size_t), 1, fin) != 1)
	{
		io_error (fin);
		return 0;
	}

	/* Read the number of samples */
	if (fread (&nsam, sizeof(size_t), 1, fin) != 1)
	{
		io_error (fin);
		return 0;
	}

	/* Initialize the new pbwt structure */
	b = pbwt_init (nsite, nsam);
	if (!b)
	{
		fclose (fin);
		return 0;
	}

	/* Read the data size */
	if (fread (&(b->datasize), sizeof(size_t), 1, fin) != 1)
	{
		io_error (fin);
		return 0;
	}

	/* Read the haplotype data */
	if (fread (b->data, sizeof(unsigned char), b->datasize, fin) != b->datasize)
	{
		io_error (fin);
		return 0;
	}

	/* Read the prefix array */
	if (fread (b->ppa, sizeof(size_t), b->nsam, fin) != b->nsam)
	{
		io_error (fin);
		return 0;
	}

	/* Read the divergence array */
	if (fread (b->div, sizeof(size_t), b->nsam, fin) != b->nsam)
	{
		io_error (fin);
		return 0;
	}

	/* Indicate pbwt is compressed */
	b->is_compress = 1;

	/* Read sample info */
	for (i = 0; i < nsam; i++)
	{
		size_t len;

		/* Read length of sample identifier string */
		if (fread (&len, sizeof(size_t), 1, fin) != 1)
		{
			io_error (fin);
			return 0;
		}

		/* Allocate heap memory for sample identifier string i */
		b->sid[i] = (char *) malloc ((len + 1) * sizeof(char));
		if (!b->sid[i])
		{
			perror ("libpbwt [ERROR]");
			return 0;
		}

		/* Read sample identifier string */
		if (fread (b->sid[i], sizeof(char), len, fin) != len)
		{
			io_error (fin);
			return 0;
		}

		/* Append null-terminating character */
		b->sid[i][len] = '\0';

		/* Read length of region string */
		if (fread (&len, sizeof(size_t), 1, fin) != 1)
		{
			io_error (fin);
			return 0;
		}

		/* Allocate head memory for region string i */
		b->reg[i] = (char *) malloc ((len + 1) * sizeof(char));
		if (!b->reg[i])
		{
			perror ("libpbwt [ERROR]");
			return 0;
		}

		/* Read region string */
		if (fread (b->reg[i], sizeof(char), len, fin) != len)
		{
			io_error (fin);
			return 0;
		}

		/* Append null-terminating character */
		b->reg[i][len] = '\0';
	}

	/* Close the input file stream */
	fclose (fin);

	return b;
}

int
pbwt_uncompress (pbwt_t *b)
{
	/* If data are already uncompressed */
	if (!b->is_compress)
		return 0;

	unsigned char *g;
    z_stream infstream;
 
    /* Allocate heap memory for uncompressed haplotype data */
    g = (unsigned char *) malloc (b->nsite * b->nsam * sizeof(unsigned char));
    if (!g)
    {
    	perror ("libpbwt [ERROR]");
    	return -1;
    }

	/* Initialize inflate stream */
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
	infstream.avail_in = b->datasize;
    infstream.next_in = (Bytef *)b->data;
    infstream.avail_out = b->nsite * b->nsam;
    infstream.next_out = (Bytef *)g;

    /* Inflate the data */
    inflateInit (&infstream);
    inflate (&infstream, Z_NO_FLUSH);
    inflateEnd (&infstream);

	/* Update pbwt data structure */
	free (b->data);
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

	unsigned char *f;
	z_stream defstream;

	/* Allocate heap memory for compressed haplotype data */
	f = (unsigned char *) malloc (b->nsam * b->nsite * sizeof(unsigned char));
	if (!f)
	{
		perror ("libpbwt [ERROR]");
		return -1;
	}

	/* Setup the deflate stream */
	defstream.zalloc = Z_NULL;
	defstream.zfree = Z_NULL;
	defstream.opaque = Z_NULL;
	defstream.avail_in = b->datasize;
	defstream.next_in = (Bytef *)b->data;
	defstream.avail_out = b->datasize;
	defstream.next_out = (Bytef *)f;

	/* Deflate the data */
	if (deflateInit (&defstream, Z_DEFAULT_COMPRESSION) != Z_OK)
	{
		fputs ("libpwbt [ERROR]: cannot initialize compression stream", stderr);
		return -1;
	}
	deflate (&defstream, Z_FINISH);
	deflateEnd (&defstream);

	/* Update pbwt data structure */
	free (b->data);
	b->is_compress = 1;
	b->data = f;
	b->datasize = defstream.total_out;

	return 0;
}

int
pbwt_print (const pbwt_t *b)
{
	/* Check if pointer is NULL */
	if (!b)
		return -1;

	size_t i;
	size_t j;

	for (i = 0; i < b->nsam; ++i)
	{
		size_t index;

		index = b->ppa[i];

		/* Print prefix and divergence array member for haplotype i */
		printf ("%5zu\t%5zu\t", b->div[i], index);

		/* Print binary haplotype array for haplotype i */
		for (j = 0; j < b->nsite; ++j)
			putchar ((char)(b->data[TWODCORD(index, b->nsite, j)]));

		/* Print sample identifier associated with haplotype i */
		if (b->sid[index])
			printf ("\t%s", b->sid[index]);
		else
		{
			fprintf (stderr, "libpbwt [ERROR]: problem reading sample identifier with index %5zu\n", index);
			return -1;
		}

		/* If a region is present */
		if (b->reg[index])
			printf ("\t%s\n", b->reg[index]);
		else
			putchar ('\n');
	}

	return 0;
}


int
build_prefix_array (pbwt_t *b)
{
	size_t i;
	size_t j;
	size_t k;
	size_t ia;
	size_t ib;
	size_t da;
	size_t db;
	size_t *ara;
	size_t *arb;
	size_t *ard;
	size_t *are;

	ara = (size_t *) malloc (b->nsam * sizeof(size_t));
	arb = (size_t *) malloc (b->nsam * sizeof(size_t));
	ard = (size_t *) malloc (b->nsam * sizeof(size_t));
	are = (size_t *) malloc (b->nsam * sizeof(size_t));

	for (i = 0; i < b->nsite; ++i)
	{
		ia = 0;
		ib = 0;
		da = i + 1;
		db = i + 1;

		for (j = 0; j < b->nsam; ++j)
		{
			size_t ix;
			size_t ms;

			ix = b->ppa[j];
			ms = b->div[j];

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
			for (j = 0; j < ia; ++j)
			{
				b->ppa[j] = ara[j];
				b->div[j] = ard[j];
			}
			for (j = 0, k = ia; j < ib; ++j, ++k)
			{
				b->ppa[k] = arb[j];
				b->div[k] = are[j];
			}
		}
	}

	/* Free allocated memory */
	free (ara);
	free (arb);
	free (ard);
	free (are);

	return 0;
}

int
find_matches (pbwt_t *b, size_t query_index, size_t minlen)
{
	size_t i;
	size_t j;
	size_t k;
	size_t ia;
	size_t ib;
	size_t da;
	size_t db;
	size_t *ara;
	size_t *arb;
	size_t *ard;
	size_t *are;

	ara = (size_t *) malloc (b->nsam * sizeof(size_t));
	arb = (size_t *) malloc (b->nsam * sizeof(size_t));
	ard = (size_t *) malloc (b->nsam * sizeof(size_t));
	are = (size_t *) malloc (b->nsam * sizeof(size_t));

	for (i = 0; i < b->nsite; ++i)
	{
		ia = 0;
		ib = 0;
		da = i + 1;
		db = i + 1;

		for (j = 0; j < b->nsam; ++j)
		{
			size_t ix;
			size_t ms;

			ix = b->ppa[j];
			ms = b->div[j];

			/*if (ms > i - minlen) */

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
			for (j = 0; j < ia; ++j)
			{
				b->ppa[j] = ara[j];
				b->div[j] = ard[j];
			}
			for (j = 0, k = ia; j < ib; ++j, ++k)
			{
				b->ppa[k] = arb[j];
				b->div[k] = are[j];
			}
		}
	}

	/* Free allocated memory */
	free (ara);
	free (arb);
	free (ard);
	free (are);

	return 0;
}


/* Definitions of non-API functions */

static int
add_match (match_t *list, size_t first, size_t second, size_t begin, size_t end)
{
	match_t *new_match;

	/* Allocate heap memory for new match */
	new_match = (match_t *) malloc (sizeof(match_t));
	if (!new_match)
	{
		perror ("libpbwt [ERROR]");
		return -1;
	}

	/* Populate the match data */
	new_match->first = first;
	new_match->second = second;
	new_match->begin = begin;
	new_match->end = end;
	new_match->next = NULL;

	/* Goto end of list and add the new element */
	if (!list)
		list = new_match;
	else
	{
		match_t *pos;
		for (pos = list; pos->next != NULL; pos = pos->next);
		pos->next = new_match;
	}

	return 0;
}

static void
io_error (FILE *f)
{
	if (ferror (f))
		fputs ("libpbwt [ERROR]: I/O failure", stderr);
	else if (feof (f))
		fputs ("libpbwt [ERROR]: truncated input file", stderr);
	fclose (f);
	return;
}
