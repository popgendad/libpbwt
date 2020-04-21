/*********************************************************************
 * libpbwt
 * A C library for implementing the positional Burrows-Wheeler transform
 *
 * Author: Daniel Garrigan <popgendad@gmail.com>
 *
 * Git: https://gitlab.com/evolgen/libpbwt
 *
 *********************************************************************/

#ifndef PBWT_H
#define PBWT_H

#include <stdlib.h>
#include <string.h>
#include <htslib/khash.h>
#include <plink_lite.h>

/* Declare hash keyed on string with double values */
KHASH_MAP_INIT_STR(floats, double)

/* 2D linearization macro */
#define TWODCORD(row, dim, col) ((row) * (dim) + (col))

/* Boolean macros */
#define FALSE 0
#define TRUE 1

/* Data structure declarations */

/* Structure to hold interval tree of matches */
typedef struct _node
{
    size_t begin;             /* The beginning position of the match */
    size_t end;               /* The end position of the match */
    size_t max;               /* Maximum end position in subtree */
    size_t count;             /* Count of matches in the interval */
    struct _node *left;       /* Pointer to the left match */
    struct _node *right;      /* Pointer to the right match */
} node_t;

/* Structure to hold the positional Burrows-Wheeler transform */
typedef struct pbwt
{
    char **sid;                  /* Diploid sample identifier string */
    char **reg;                  /* Region/population of sample */
    char **chr;                  /* Chromosome identifer of all SNPs in pbwt */
    char **rsid;                 /* RSID for all SNPs in pbwt */
    double *cm;                  /* Genetic map positions for all SNPs in pbwt */
    unsigned char *data;         /* Binary haplotype representation */
    unsigned char is_compress;   /* Are haplotype data compressed? */
    unsigned char *is_query;     /* Is the haplotype a query sequence? */
    size_t datasize;             /* Number of bytes stored in data */
    size_t nsite;                /* Number of sampled sites */
    size_t nsam;                 /* Number of sampled haplotypes */
    node_t *intree;              /* Pointer to interval tree of matches */
    khash_t(floats) *reghash;    /* Pointer to the region hash table */
    double **cmatrix;            /* Pointer to the coancestry matrix */
} pbwt_t;


/* libpbwt API function declarations */

extern pbwt_t *pbwt_init(const size_t, const size_t);

extern void pbwt_destroy(pbwt_t *);

extern int pbwt_compress(pbwt_t *);

extern int pbwt_uncompress(pbwt_t *);

extern int pbwt_write(const char *, pbwt_t *);

extern int pbwt_push(pbwt_t *, const char *, const char *, const char *, const char *);

extern pbwt_t *pbwt_remove(pbwt_t *, const khash_t(integer) *);

extern pbwt_t *pbwt_copy(pbwt_t *);

extern pbwt_t *pbwt_merge(pbwt_t *, pbwt_t *);

extern pbwt_t *pbwt_read(const char *);

extern pbwt_t *pbwt_import_plink(const char *);

extern pbwt_t *pbwt_import_vcf(const char *, const char *);

extern char **pbwt_get_reglist(pbwt_t *, size_t *);

extern khash_t(integer) *pbwt_get_sampdict(const pbwt_t *);

extern khash_t(integer) *pbwt_get_regcount(const pbwt_t *);

extern size_t *pbwt_build(const pbwt_t *);

extern pbwt_t *pbwt_subset(pbwt_t *, const khash_t(integer) *);

extern pbwt_t *pbwt_subset_with_query(pbwt_t *, const char *, const size_t);

extern int pbwt_set_match(pbwt_t *, const double, void (*report)(pbwt_t *, const size_t, const size_t, const size_t, const size_t));

extern int pbwt_all_match(pbwt_t *, const double, void (*report)(pbwt_t *, const size_t, const size_t, const size_t, const size_t));

extern int pbwt_all_query_match(pbwt_t *, const double, void (*report)(pbwt_t *, const size_t, const size_t, const size_t, const size_t));

extern int pbwt_set_query_match(pbwt_t *, const double, void (*report)(pbwt_t *, const size_t, const size_t, const size_t, const size_t));

extern void tree_print(pbwt_t *, node_t *);

extern node_t *match_insert(node_t *, const size_t, const size_t, const size_t, const size_t);

extern void match_count(pbwt_t *, node_t *, size_t *, const size_t, const size_t);

extern const char *libpbwt_version(void);

#endif
