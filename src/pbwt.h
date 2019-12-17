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

/* Weighted edge */
typedef struct _edge
{
    size_t index;
    double weight;
    struct _edge *next;
} edge_t;

/* Graph vertex */
typedef struct _vertex
{
    size_t numconnect;
    char *sampid;
    char *pop;
    edge_t *head;
} vertex_t;

/* Undirected adjacency list with weighted edges */
typedef struct _adjlist
{
    size_t n_vertices;
    vertex_t *nodelist;
} adjlist_t;

/* Structure to hold interval tree of matches */
typedef struct _match
{
    size_t first;             /* The original index of the first matching haplotype */
    size_t second;            /* The original index of the second matching haplotype */
    size_t begin;             /* The beginning position of the match */
    size_t end;               /* The end position of the match */
    size_t max;               /* Maximum end position in subtree */
    struct _match *left;      /* Pointer to the left match */
    struct _match *right;     /* Pointer to the right match */
} match_t;

/* Structure to hold the positional Burrows-Wheeler transform */
typedef struct pbwt
{
    char **sid;               /* Diploid sample identifier string */
    char **reg;               /* Region/population of sample */
    char **chr;               /* Chromosome identifer of all SNPs in pbwt */
    char **rsid;              /* RSID for all SNPs in pbwt */
    double *cm;               /* Genetic map positions for all SNPs in pbwt */
    unsigned char *data;      /* Binary haplotype representation */
    int is_compress;          /* Are haplotype data compressed? */
    int *is_query;            /* Is the haplotype a query sequence? */
    size_t datasize;          /* Number of bytes stored in data */
    size_t nsite;             /* Number of sampled sites */
    size_t nsam;              /* Number of sampled haplotypes */
    size_t *ppa;              /* Pointer to the prefix array */
    size_t *div;              /* Pointer to the divergence array */
    match_t *match;           /* Pointer to set-proximal match linked list */
} pbwt_t;


/* libpbwt API function declarations */

extern pbwt_t *pbwt_init(const size_t, const size_t);

extern void pbwt_destroy(pbwt_t *);

extern int pbwt_compress(pbwt_t *);

extern int pbwt_uncompress(pbwt_t *);

extern int pbwt_write(const char *, pbwt_t *);

extern int pbwt_push(pbwt_t *, const char *, const char *, const char *, const char *);

extern pbwt_t *pbwt_pull(pbwt_t *, const size_t);

extern pbwt_t *pbwt_copy(pbwt_t *);

extern pbwt_t *pbwt_merge(pbwt_t *, pbwt_t *);

extern pbwt_t *pbwt_read(const char *);

extern pbwt_t *pbwt_import_plink(const char *);

extern pbwt_t *pbwt_import_vcf(const char *, const char *);

extern char **pbwt_get_reglist(pbwt_t *, size_t *);

extern khash_t(integer) *pbwt_get_sampdict(pbwt_t *);

extern khash_t(integer) *pbwt_get_regcount(pbwt_t *);

extern int pbwt_build(pbwt_t *);

extern pbwt_t *pbwt_subset(pbwt_t *, const char *);

extern pbwt_t *pbwt_subset_with_query(pbwt_t *, const char *, const size_t);

extern int pbwt_set_match(pbwt_t *, const double);

extern int pbwt_find_match(pbwt_t *, const size_t);

extern int pbwt_query_match(pbwt_t *, const double);

extern int match_regsearch(pbwt_t *, match_t *, khash_t(floats) *, size_t, size_t);

extern int match_coasearch(pbwt_t *, match_t *, double **, size_t, size_t);

extern int match_adjsearch(pbwt_t *, match_t *, adjlist_t *, size_t, size_t);

extern void match_print(pbwt_t *, match_t *);

extern void elementary_print(pbwt_t *, match_t *);

extern match_t *match_insert(match_t *, const size_t, const size_t, const size_t, const size_t);

extern size_t match_count(pbwt_t *, match_t *, double *);

extern match_t *match_new(const size_t, const size_t, const size_t, const size_t);

extern int match_overlap(const size_t, const size_t, const size_t, const size_t);

extern adjlist_t *create_adjlist(const size_t, char **, char **);

extern void add_edge(adjlist_t *, double, size_t, size_t);

extern void print_adjlist(adjlist_t *g);

extern adjlist_t *diploidize(adjlist_t *);

extern const char *pbwt_version(void);

#endif
