# libpbwt

A C library for implementing the positional Burrows-Wheeler transform (PBWT)

## Introduction

The positional Burrows-Wheeler transform is an efficient method for haplotype storage and matching ([Durbin 2014](https://www.ncbi.nlm.nih.gov/pubmed/24413527)). The `libpbwt` library introduces a new file format for storing PBWT data, which is described below in the [PBWT file format](https://gitlab.com/evolgen/libpbwt#PBWT-file-format) subsection below.

The [`pbwtmaster`](https://gitlab.com/evolgen/pbwtmaster) program is a front-end utility that uses a large proportion of the functionality provided in the `libpbwt` library.

## Installation

A `Makefile` is provided with the `libpbwt` package. There are three dependencies outside of the standard C library, including

1. the DEFLATE compression library `zlib`

2. the PLINK format I/O routines in `libplink_lite`

3. the VCF I/O routines in `htslib`

The default installation directories are `/usr/lib` for the both the static and shared library files (`libpbwt.a` and `libpbwt.so` respectively) and `/usr/include` for the `pbwt.h` header file. Below is an example of installing the library.

```
git clone git@gitlab.com/evolgen/libpbwt.git
cd libpbwt
make all
sudo make install
make clean
```

## Linking the Library

### Static Library

Include the `/usr/lib/libpbwt.a` file.

```
gcc -O2 -Wall -o ptest ptest.c /usr/lib/libpbwt.a
```

### Shared Library

Add the `-lpbwt` linker statement during compilation.

```
gcc -O2 -Wall -o ptest ptest.c -lpbwt
```

## PBWT File Format

Files with the `.pbwt` extension are binary files that store a single instance of the `pbwt_t` data structure. The haplotype data stored in the `pbwt::data` variable are compressed when the data structure is written to file. The `.pbwt` format can be written and read with the `pbwt_write()` and `pbwt_read()` functions, respectively, provided by the `libpbwt` library.

### File Structure

Below is a table of the data format of a `.pbwt` file, in order that they are are stored.

| description | data type | libpbwt variable name |
| ----------- | --------- | --------------------- |
| Number of sites in alignment | `size_t` | `pbwt::nsite` |
| Number of haplotypes | `size_t` | `pbwt::nsam` |
| Size of compressed genotype data | `size_t` | `pwbt::datasize` |
| Compressed genotype data | `unsigned char * pbwt::datasize` | `pbwt::data` |

Following these data are the variable-length strings, each string is preceeded by an unsigned integer specifying its length:

The haplotype identifiers:

```
foreach i in pbwt::nsam
   "Length of haplotype identifier string", size_t, strlen(pbwt::sid[i])
   "Haplotype identifier string", char * strlen(pbwt::sid[i]), pbwt::sid[i]
```

The haplotype regions:

```
foreach i in pbwt::nsam
   "Length of region identifier string", size_t, strlen(pbwt::reg[i])
   "Region identifer string", char * strlen(pbwt::reg[i]), pbwt::reg[i]
```

And then the chromosome/scaffold, genetic map position and RSID of each site:

```
foreach j in pbwt::nsite
   "Length of RSID string", size_t, strlen(pbwt::rsid[j])
   "RSID string" char * strlen(pbwt::rsid[j]), pbwt::rsid[j]
   "Length of chromosome ID string", size_t, strlen(pbwt::chr[j])
   "Chromosome identifier", char * strlen(pbwt::chr[j]), pbwt::chr[j]
   "Map position for marker j", double, pbwt::cm[j]
```

### Compression Metrics

#### Compression of Haplotype Data

Using the DEFLATE compression algorithm on the haplotype data achieves a mean compression that reduces the size to 0.15 of the original data size.


## Data Types

### pbwt_t

The main data type is called `pbwt_t` and it stores all of the information associated with the data contributing to the to haplotype alignment of interest. The `pbwt_t` is declared as

```c
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
```

### match_t

The `match_t` data type stores match coordinates found within the pbwt in an interval tree. The interval tree can be easily searched for coverage and depth. The `match_t` type is declared as

```c
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
```

### Graph-based Data Structures

#### edge_t

```c
typedef struct _edge
{
    size_t index;
    double weight;
    struct _edge *next;
} edge_t;
```

#### vertex_t

```c
typedef struct _vertex
{
    size_t numconnect;
    char *sampid;
    char *pop;
    edge *head;
} vertex_t;
```

#### adjlist_t

```c
typedef struct _adjlist
{
    size_t n_vertices;
    vertex *nodelist;
} adjlist_t;
```

## API Functions

### Create/Destroy Data Structures

#### pbwt_init()

```c
pbwt_t *pbwt_init(const size_t nsite, const size_t nsam)
```

The `pbwt_t` data structure can be initialized using the `pbwt_init()` function.

The above function takes the number of sites (`nsite`) and the number of haplotypes (`nsam`) and returns an empty pbwt structure of size `nsam` x `nsite`. The function will return a `NULL` pointer if it encounters a problem initializing the data structure. The new data structure has memory allocated for the uncompressed binary haplotype data (`data`), the sample and region identifiers (`sid` and `reg`, respectively), the prefix array (`ppa`) and the divergence array (`div`). The `match` member variable is not initialized at this time and remains `NULL`.

#### pbwt_destroy()

```c
void pbwt_destroy(pbwt_t *b)
```

This function deallocates all memory contained in the `pbwt_t` data structure referred to by `b`. The variable `b` cannot be re-used after `pbwt_destroy()` is called, rather it must be re-initialized with the `pbwt_init()` function. This function does not return a value.

#### pbwt_version()

```c
const char *pbwt_version(void)
```

The `pbwt_version` function returns a pointer to a constant character array holding the version number of the library. This function does not take any arguments.


### I/O Functions

#### pbwt_read()

```c
pbwt_t *pbwt_read(const char *infile)
```

The `pbwt_read()` function reads a `.pbwt` format file into memory and returns a pointer to the `pbwt_t` data structure contained in `infile`. The function returns a `NULL` pointer if it encounters a problem reading the file.

#### pbwt_import_plink()

```c
pbwt_t *pbwt_import_plink(const char *plinkstub)
```

The `pbwt_import_plink` function returns a `pbwt_t` data structure populated from data imported from a plink-formatted data set specified by `plinkstub` (e.g, `.bed`, `bim`, `.fam`), plus a `.reg` file. The function return a `NULL` pointer if it encounters an error during the import process.

#### pbwt_import_vcf()

```c
pbwt_t *pbwt_import_vcf(const char *vcfinfile, const char *popmap);
```

The `pbwt_import_vcf` function returns a `pbwt_t` data structure population from the VCF infile specified as `vcfinfile` and the popmap file `popmap` (e.g., two columns with sample identifier and population). The function return a `NULL` pointer if it encounters an error during the import process.

#### pbwt_write()

```c
int pbwt_write(const char *outfile, pbwt_t *b)
```

The above function will write the `pbwt_t` data structure pointed to by `b` and write it to a `.pbwt` format file named `outfile`. If a file by the name specified by `outfile` exists on disk, the function will overwrite that file. If it does not have permission to overwrite that file, the function will return a value of -1. The function will compress the binary haplotype data contained in the `pbwt_t` data structure before writing the file. The function will return 0 on success and -1 on error.


### Functions Operating on PBWT

#### pbwt_build()

```c
int pbwt_build(pbwt_t *b)
```

The `pbwt_build()` function is based on algorithm 2 of Durbin (2014) and takes an initialized `pbwt_t` data structure that already contains the raw binary haplotype data and constructs both the prefix and divergence arrays up to the site at index position `nsite - 1`. The function will return 0 on success and -1 on error.

#### pbwt_subset()

```c
pbwt_t *pbwt_subset(pbwt_t *b, const char *reg)
```

Subsets the `pbwt_t` data structure pointed to by `b` and creates a new `pbwt_t` data structure that is subset to haplotypes with region label `reg`. Returns a pointer to the new `pbwt_t` data structure on success and a `NULL` pointer on failure.

#### pbwt_subset_with_query()

```c
pbwt_t *pbwt_subset_with_query(pbwt_t *b, const char *reg, const size_t query)
```

Similar to `pbwt_subset()` except it includes an additional query sequence with index `query`.

#### pbwt_push()

```c
int pbwt_push(pbwt_t *b, const char *new_sid, const char *new_reg, const char *h1, const char *h2)
```

The `pbwt_push()` function adds two query sequences to the `pbwt_t` data structure pointed to by `b` and reconstructs the prefix and divergence arrays. In addition to a pointer to the `pbwt_t` data structure to be augmented, the function takes arguments that include a pointer to the new sample identifier `new_sid`, a pointer to the new sample region `new_reg` (which can be `NULL`), a pointer to the first binary haplotype array `h1` and the second haplotype array `h2`. The function will return 0 on success and -1 on error.

#### pbwt_pull()

```c
pbwt_t *pbwt_pull(pbwt_t *b, const size_t target)
```

#### pbwt_copy()

```c
pbwt_t *pbwt_copy(pbwt_t *b)
```

Returns a separate copy of the `pbwt_t` data structure pointed to by `b`. The function will return a `NULL` pointer on failure.


#### pbwt_merge()

```c
pbwt_t *pbwt_merge(pbwt_t *b1, pbwt_t *b2)
```

Creates a new `pbwt_t` data structure by merging two other `pbwt_t` strucutres and returns a pointer to the merged strucutre. The function will return a `NULL` pointer on failure.


### Miscellaneous

#### pbwt_get_reglist()

```c
char **pbwt_get_reglist(pbwt_t *b, size_t *nr)
```

Extracts a unique list of regions from the `pbwt_t` data structure pointed to by `b`. The total number of unique regions is stored in the memory pointed to by `nr` and a pointer to the string array is returned on success, while a `NULL` pointer is returned on failure.


#### pbwt_get_regcount()

```c
khash_t(integer) *pbwt_get_regcount(pbwt_t *b)
```

Returns a hash table keyed on region identifier strings and the associated value is the count of the number of samples in the PBWT from that region. The function will return a `NULL` pointer on failure.


#### pbwt_get_sampdict()

```c
khash_t(integer) *pbwt_get_sampdict(pbwt_t *b)
```

Returns a hash table keyed on sample identifier strings and the associated value is the index of the sample in the PBWT. The function will return a `NULL` pointer on failure.


### Matching

#### pbwt_all_match()
```c
int pbwt_all_match(pbwt_t *b, const double minlen)
```

The `pbwt_all_match` is algorithm 3 of Durbin (2014). It finds all matches that are longer than `minlen`
cM in a pbwt matrix and stores them in an interval tree pointed to by `b->match`. The function will return 0 on success and -1 on error.


#### pbwt_all_query_match()
```c
int pbwt_all_query_match(pbwt_t *b, const double minlen)
```

The `pbwt_all_query_match` is algorithm 3 of Durbin (2014). It finds all matches that are longer than
`minlen` cM in a pbwt matrix and stores them in an interval tree pointed to by `b->match`. The function will return 0 on success and -1 on error.


#### pbwt_set_match()

```c
int pbwt_set_match(pbwt_t *b, double minlen)
```

The `pbwt_set_match` function finds the set maximal matches in `b`. The minimum length required to be considered a match is specified by the `minlen` variable. The minimum length is the total genetic map distance (cM) covered by a potential match. The function returns -1 on error and zero on success. A pointer to the match interval tree is stored in `b->match`.


#### pbwt_set_query_match()
```c
int pbwt_set_query_match(pbwt_t *b, const size_t query_index, const double minlen)
```

The `pbwt_set_query_match` function finds only the set maximal matches in `b` that involve the haplotypes labeled as query sequences. The minimum length required to be considered a match is specified by the `minlen` variable. The minimum length is the total genetic map distance (cM) of the window covered by a potential match. The function returns -1 on error and zero on success. A pointer to the match interval tree is stored in `b->match`.

#### match_adjsearch()

```c
int match_adjsearch(pbwt_t *b, match_t *node, adjlist_t *, size_t qbegin, size_t qend)
```

The `match_adjsearch` function stores the list of all matches overlapping the interval `qbegin` to `qend` in an adjacency list data structure. The function will return 0 on success and -1 on error.

#### match_coasearch()

```c
int match_coasearch(pbwt_t *b, match_t *node, double **cmatrix, size_t qbegin, size_t qend, int is_diploid)
```

The `match_coasearch` function stores a list of all matches overlapping the interval `qbegin` to `qend` in a pairwise coancestry matrix. The `is_diploid` flag indicates that the dimension of the `cmatrix` is `b->nsam` / 2. The function will return 0 on success and -1 on error.

#### match_regsearch()

```c
int match_regsearch(pbwt_t *b, match_t *node, khash(double) *h, size_t qbegin, size_t qend)
```

The `match_regsearch` function stores a list of all matches overlapping the interval `qbegin` to `qend` into a hash that is keyed on the region/population with the value being the sum of the recombination distance of all matches between the query haplotype and the keyed region. The function will return 0 on success and -1 on error.

#### match_print()

```c
void match_print(pbwt_t *b, match_t *node)
```

Dumps all reported matches to `stdout`. This function does not return a value.


### Graphs Representations of Matches

#### create_adjlist()

```c
adjlist_t *create_adjlist(const size_t V, char **samplist, char **reglist)
```

Creates an adjacency list representing a graph with `V` vertices, whose string identifiers are stored in `samplist` and whose associated regions are store in `reglist`. On success, the function returns an `adjlist_t` data structure and a `NULL` pointer on failure.

#### allocate_new_edge()

```c
edge_t *allocate_new_edge(const size_t partner, const double w)
```

Allocates a new edge with index `partner` and weight `w`. On success, the function returns a pointer to the edge and a `NULL` pointer on failure.

#### diploidize()

```c
adjlist_t *diploidize(adjlist_t *g)
```

Creates a new adjacency matrix by joining the two haplotypes in adjacency list `g` from a single diploid individual and returns the merged `adjlist_t` object on success and a `NULL` pointer on failure.

#### print_adjlist()

```c
void print_adjlist(adjlist_t *g)
```

Prints an adjacency list to `stdout`. For each edge in the graph represented by `g`, there are five columns, corresponding to:

1. Sample identifier string of vertex
2. Sample identifier string of matching vertex
3. Edge weight in cM
4. Population/region of sample
5. Population/region of matching sample

The `print_adjlist` function does not return a value.


## Examples

The example below reads a PLINK-format file and converts it to the `pbwt_t` data structure and writes it to a PBWT-format file.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pbwt.h>

int main (int argc, char *argv[])
{
    size_t length;
    char *outfile;
    pbwt_t *b;
    char *instub = strdup(argv[1]);

    /* Construct outfile name */
    length = strlen(instub);
    outfile = (char *)malloc((length + 4) * sizeof(char));
    strcpy(outfile, instub);
    strcat(outfile, ".pbwt");

    /* Import PBWT structure */
    b = pbwt_import_plink(c->instub);

    /* Build the prefix and divergence arrays */
    pbwt_build(b);

    /* Write the pbwt to file */
    pbwt_write(outfile, b);

    /* Free memory for the data structure */
    pbwt_destroy(b);

    return 0;
}
```

## Contributing

### TODO

1. Translate to C++

2. Better error signal handling

3. Finish writing `pbwt_pull` function
