# libpbwt

A C library for implementing the positional Burrows-Wheeler transform

## Introduction

The position Burrows-Wheeler transform is an efficient method for haplotype storage and matching ([Durbin 2014](https://www.ncbi.nlm.nih.gov/pubmed/24413527)). The `libpbwt` library introduces a new file format for storing pbwt data, which is described below in the [pbwt file format](##pbwt-file-format) subsection below.

## Installation

A Makefile is provided with the `libpbwt` package. The only dependency outside of the standard C library is `zlib`. The default installation directories are `/usr/lib` for the shared library file `libpbwt.so` and `/usr/include` for the `pbwt.h` header file. Below is an example of installing the library.

```
git clone git@github.ancestry.com:DNA-Science/libpbwt.git
cd libpbwt
make
sudo make install
```

## Linking the library

Coming soon

## pbwt file format

Files with the `.pbwt` extension are binary files that store a single instance of the `pbwt_t` data structure. The haplotype data stored in the `pbwt::data` variable are compressed when the data structure is written to file. The `.pbwt` format can be written and read with the `pbwt_write()` and `pbwt_read()` functions, respectively, provided by the `libpbwt` library.

### File structure

Below is a table of the data format of a `.pbwt` file, in order that they are are stored.

| description | data type | libpbwt variable name |
| ----------- | --------- | --------------------- |
| Number of sites in alignment | `size_t` | `pbwt::nsite` |
| Number of haplotypes | `size_t` | `pbwt::nsam` |
| Size of compressed genotype data | `size_t` | `pwbt::datasize` |
| Compressed genotype data | `unsigned char * pbwt::datasize` | `pbwt::data` |
| Prefix array | `size_t * pbwt::nsam` | `pbwt::ppa` |
| Divergence array | `size_t * pbwt::nsam` | `pbwt::div` |

Following these data are the variable-length strings, in which each string is preceeded by its length:

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

And then the chromosome, genetic map position and RSID of each site:

```
foreach j in pbwt::nsite
   "Length of RSID string", size_t, strlen(pbwt::rsid[j])
   "RSID string" char * strlen(pbwt::rsid[j]), pbwt::rsid[j]
   "Chromosome identifier", int, pbwt::chr[j]
   "Map position for marker j", double, pbwt::cm[j]
```

### Compression metrics

Coming soon

## Data types

### pbwt_t

The main data type is called `pbwt_t` and it stores all of the information associated with the data contributing to the to haplotype alignment of interest. The `pbwt_t` is declared as

```
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
    match_t *match;           /* Pointer to match data structure */
} pbwt_t;
```

### match_t

The `match_t` data type stores match coordinates found within the pbwt in a linked list. The `match_t` type is declared as

```
typedef struct _match
{
    size_t first;             /* The original index of the first matching haplotype */
    size_t second;            /* The original index of the second matching haplotype */
    size_t begin;             /* The beginning position of the match */
    size_t end;               /* The end position of the match */
    struct _match *next;      /* Pointer to the next match */
} match_t;
```

## API functions

### Create/Destroy data structures

#### pbwt_init()

```
pbwt_t * pbwt_init (const size_t nsite, const size_t nsam)
```

The `pbwt_t` data structure can be initialized using the `pbwt_init()` function.

The above function takes the number of sites (`nsite`) and the number of haplotypes (`nsam`) and returns an empty pbwt structure of size `nsam` x `nsite`. The function will return a `NULL` pointer if it encounters a problem initializing the data structure. The new data structure has memory allocated for the uncompressed binary haplotype data (`data`), the sample and region identifiers (`sid` and `reg`, respectively), the prefix array (`ppa`) and the divergence array (`div`). The `match` member variable is not initialized at this time and remains `NULL`.

#### pbwt_destroy()

```
void pbwt_destroy (pbwt_t *b)
```

This function deallocates all memory contained in the `pbwt_t` data structure referred to by `b`. The variable `b` cannot be re-used after `pbwt_destroy()` is called, rather it must be re-initialized with the `pbwt_init()` function. This function does not return a value.

### I/O functions

#### pbwt_read()

```
pbwt_t * pbwt_read (const char *infile)
```

The `pbwt_read()` function reads a `.pbwt` format file into memory and returns a pointer to the `pbwt_t` data structure contained in that file. The full name of the input file is given in the `infile` variable. The function returns a `NULL` pointer if it encounters a problem reading the file.

#### pbwt_write()

```
int pbwt_write (const char *outfile, pbwt_t *b)
```

The above function will write the `pbwt_t` data structure pointed to by `b` and write it to a `.pbwt` format file named `outfile`. If a file by the name specified by `outfile` exists on disk, the function will overwrite that file. If it does not have permission to overwrite that file, the function will return a value of -1. The function will compress the binary haplotype data contained in the `pbwt_t` data structure before writing the file. The function will return 0 on success and -1 on error.

#### pbwt_print()

```
int pbwt_print (const pbwt_t *b)
```

The `pbwt_print()` function will print a representation of the `pbwt_t` data structure to `stdout`. The function will return 0 on success and -1 on failure.

### Functions that operate on pbwt

#### pbwt_build()

```
int pbwt_build (pbwt_t *b)
```

The `pbwt_build()` function is based on algorithm 2 of Durbin (2014) and takes an initialized `pbwt_t` data structure that already contains the raw binary haplotype data and constructs both the prefix and divergence arrays up to the site at index position `nsite - 1`. The function will return 0 on success and -1 on error.

#### pbwt_add()

```
int pbwt_add (pbwt_t *b, const char *new_sid, const char *new_reg, const char *h1, const char *h2)
```

The `pbwt_add()` function adds two query sequences to the `pbwt_t` data structure pointed to by `b` and reconstructs the prefix and divergence arrays. In addition to a pointer to the `pbwt_t` data structure to be augmented, the function takes arguments that include a pointer to the new sample identifier `new_sid`, a pointer to the new sample region `new_reg` (which can be `NULL`), a pointer to the first binary haplotype array `h1` and the second haplotype array `h2`.

### Matching

#### pbwt_match()

```
int pbwt_match (pbwt_t *b, size_t query_index, double minlen)
```

The `pbwt_match` function finds all the matches in `b` that match the haplotype indexed by `query_index`. The minimum length required to be considered a match is specified by the `minlen` variable. The minimum length is the proportion of the total genetic map distance of the window covered by a potential match. The results are stored in the linked list in `b->match`.

#### pbwt_print_matches()

```
int pbwt_print_match (pbwt_t *b, match_t *list)
```

Dumps all reported matches to `stdout`.

## Examples

Coming soon

## Contributing

Coming soon
