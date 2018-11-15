#ifndef PBWT_H
#define PBWT_H

/* 2D linearization macro */
#define TWODCORD(row, dim, col) ((row) * (dim) + (col))

/* Boolean macros */
#define FALSE 0
#define TRUE 1

/* Data structure declarations */

/* Structure to hold linked list of matches */
typedef struct _match
{
    size_t first;             /* The original index of the first matching haplotype */
    size_t second;            /* The original index of the second matching haplotype */
    size_t begin;             /* The beginning position of the match */
    size_t end;               /* The end position of the match */
    struct _match *next;      /* Pointer to the next match */
} match_t;

/* Structure to hold the positional Burrows-Wheeler transform */
typedef struct pbwt
{
    char **sid;               /* Diploid sample identifier string */
    char **reg;               /* Region/population of sample */
    unsigned char *data;      /* Binary haplotype representation */
    int is_compress;          /* Are haplotype data compressed? */
    size_t datasize;          /* Number of bytes stored in data */
    size_t nsite;             /* Number of sampled sites */
    size_t nsam;              /* Number of sampled haplotypes */
    size_t *ppa;              /* Pointer to the prefix array */
    size_t *div;              /* Pointer to the divergence array */
    match_t *match;           /* Pointer to set-proximal match linked list */
} pbwt_t;


/* libpbwt API function declarations */

extern pbwt_t *pbwt_init (const size_t, const size_t);

extern void pbwt_destroy (pbwt_t *);

extern int pbwt_compress(pbwt_t *);

extern int pbwt_uncompress (pbwt_t *);

extern int pbwt_write (const char *, pbwt_t *);

extern int pbwt_add (pbwt_t *, const char *, const char *, const char *, const char *);

extern pbwt_t *pbwt_read (const char *);

extern int pbwt_print (const pbwt_t *);

extern int pbwt_build (pbwt_t *);

extern match_t *pbwt_match (pbwt_t *, const size_t, const size_t);

extern int pbwt_print_match (match_t *);

extern pbwt_t * pbwt_subset (pbwt_t *, const char *);

#endif
