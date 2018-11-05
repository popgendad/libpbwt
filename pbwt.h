#ifndef PBWT_H
#define PBWT_H

/* Data structure definitions */
typedef struct pbwt
{
	char **sid;               /* Sample identifier string */
	char **reg;               /* Region/population of sample */
	unsigned char **data;     /* Haplotype representation */
	size_t nsite;             /* Number of sites */
	size_t nsam;              /* Number of haplotypes */
	size_t *ppa;              /* Prefix array */
	size_t *div;              /* Divergence array */
} pbwt_t;

/* Function declarations */
pbwt_t *pbwt_init (const size_t, const size_t);
int pbwt_destroy (pbwt_t *);
int pbwt_print (const pbwt_t *);
int build_prefix_array (pbwt_t *);
int report_set_maximal_matches(pbwt_t *);
int report_long_matches(pbwt_t *);

#endif