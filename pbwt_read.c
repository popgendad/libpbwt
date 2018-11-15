#include <stdlib.h>
#include <stdio.h>
#include "pbwt.h"

static void io_error (FILE *);

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
    if (fin == NULL)
    {
        perror ("libpbwt [ERROR]");
        return NULL;
    }

    /* Read the data into memory */
    /* First read the number of sites */
    if (fread (&nsite, sizeof(size_t), 1, fin) != 1)
    {
        io_error (fin);
        return NULL;
    }

    /* Read the number of samples */
    if (fread (&nsam, sizeof(size_t), 1, fin) != 1)
    {
        io_error (fin);
        return NULL;
    }

    /* Initialize the new pbwt structure */
    b = pbwt_init (nsite, nsam);
    if (b == NULL)
    {
        fclose (fin);
        return NULL;
    }

    /* Read the data size */
    if (fread (&(b->datasize), sizeof(size_t), 1, fin) != 1)
    {
        io_error (fin);
        return NULL;
    }

    /* Read the haplotype data */
    if (fread (b->data, sizeof(unsigned char), b->datasize, fin) != b->datasize)
    {
        io_error (fin);
        return NULL;
    }

    /* Read the prefix array */
    if (fread (b->ppa, sizeof(size_t), b->nsam, fin) != b->nsam)
    {
        io_error (fin);
        return NULL;
    }

    /* Read the divergence array */
    if (fread (b->div, sizeof(size_t), b->nsam, fin) != b->nsam)
    {
        io_error (fin);
        return NULL;
    }

    /* Indicate pbwt is compressed */
    b->is_compress = TRUE;

    /* Read sample info */
    for (i = 0; i < nsam; ++i)
    {
        size_t len;

        /* Read length of sample identifier string */
        if (fread (&len, sizeof(size_t), 1, fin) != 1)
        {
            io_error (fin);
            return NULL;
        }

        /* Allocate heap memory for sample identifier string i */
        b->sid[i] = (char *) malloc ((len + 1) * sizeof(char));
        if (b->sid[i] == NULL)
        {
            perror ("libpbwt [ERROR]");
            return NULL;
        }

        /* Read sample identifier string */
        if (fread (b->sid[i], sizeof(char), len, fin) != len)
        {
            io_error (fin);
            return NULL;
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
        if (b->reg[i] == NULL)
        {
            perror ("libpbwt [ERROR]");
            return NULL;
        }

        /* Read region string */
        if (fread (b->reg[i], sizeof(char), len, fin) != len)
        {
            io_error (fin);
            return NULL;
        }

        /* Append null-terminating character */
        b->reg[i][len] = '\0';
    }

    /* Close the input file stream */
    fclose (fin);

    return b;
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
