#ifndef PBWT_H
#define PBWT_H

#include <zlib.h>

#define TWODCORD(row, dim, col) ((row) * (dim) + (col))


/* Data structure definitions */
typedef struct pbwt
{
	char **sid;               /* Sample identifier string */
	char **reg;               /* Region/population of sample */
	unsigned char *data;      /* Haplotype representation */
	int is_compress;          /* Are haplotype data compressed? */
	size_t datasize;          /* Number of bytes stored in data */
	size_t nsite;             /* Number of sites */
	size_t nsam;              /* Number of haplotypes */
	size_t *ppa;              /* Prefix array */
	size_t *div;              /* Divergence array */
} pbwt_t;


/* Function declarations */

pbwt_t *pbwt_init (const size_t, const size_t);

int pbwt_destroy (pbwt_t *);

int pbwt_compress(pbwt_t *);

int pbwt_uncompress (pbwt_t *);

int pbwt_write (const char *, pbwt_t *);

pbwt_t *pbwt_read (const char *);

int pbwt_print (const pbwt_t *);

int build_prefix_array (pbwt_t *);

int report_set_maximal_matches (pbwt_t *);

int report_long_matches (pbwt_t *);

#endif
