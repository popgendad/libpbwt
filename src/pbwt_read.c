#include <stdio.h>
#include "pbwt.h"

static void io_error(FILE *);

pbwt_t *pbwt_read(const char *infile)
{
    size_t i = 0;
    size_t j = 0;
    size_t r = 0;
    size_t nsite = 0;
    size_t nsam = 0;
    pbwt_t *b = NULL;
    FILE *fin;

    /* Open binary input file stream */
    fin = fopen(infile, "rb");
    if (fin == NULL)
    {
        return NULL;
    }

    /* Read the data into memory */
    /* First read the number of sites */
    r = fread(&nsite, sizeof(size_t), 1, fin);
    if (r != 1)
    {
        io_error(fin);
        return NULL;
    }

    /* Read the number of samples */
    r = fread(&nsam, sizeof(size_t), 1, fin);
    if (r != 1)
    {
        io_error(fin);
        return NULL;
    }

    /* Initialize the new pbwt structure */
    b = pbwt_init(nsite, nsam);
    if (b == NULL)
    {
        fclose(fin);
        return NULL;
    }

    /* Read the data size */
    r = fread(&(b->datasize), sizeof(size_t), 1, fin);
    if (r != 1)
    {
        io_error(fin);
        return NULL;
    }

    /* Read the haplotype data */
    r = fread(b->data, sizeof(unsigned char), b->datasize, fin);
    if (r != b->datasize)
    {
        io_error(fin);
        return NULL;
    }

    /* Indicate pbwt is compressed */
    b->is_compress = TRUE;

    /* Read sample info */
    for (i = 0; i < nsam; ++i)
    {
        size_t len = 0;

        /* Read length of sample identifier string */
        r = fread(&len, sizeof(size_t), 1, fin);
        if (r != 1)
        {
            io_error(fin);
            return NULL;
        }

        /* Allocate heap memory for sample identifier string i */
        b->sid[i] = (char *)malloc((len + 1) * sizeof(char));
        if (b->sid[i] == NULL)
        {
            return NULL;
        }

        /* Read sample identifier string */
        r = fread(b->sid[i], sizeof(char), len, fin);
        if (r != len)
        {
            io_error(fin);
            return NULL;
        }

        /* Append null-terminating character */
        b->sid[i][len] = '\0';

        /* Read length of region string */
        r = fread(&len, sizeof(size_t), 1, fin);
        if (r != 1)
        {
            io_error(fin);
            return 0;
        }

        /* Allocate head memory for region string i */
        b->reg[i] = (char *)malloc((len + 1) * sizeof(char));
        if (b->reg[i] == NULL)
        {
            return NULL;
        }

        /* Read region string */
        r = fread(b->reg[i], sizeof(char), len, fin);
        if (r != len)
        {
            io_error(fin);
            return NULL;
        }

        /* Append null-terminating character */
        b->reg[i][len] = '\0';
    }

    for (j = 0; j < b->nsite; ++j)
    {
        size_t len = 0;

       /* Read length of RSID string */
       r = fread(&len, sizeof(size_t), 1, fin);
        if (r != 1)
        {
            io_error(fin);
            return 0;
        }

        /* Allocate head memory for RSID string j */
        b->rsid[j] = (char *)malloc((len + 1) * sizeof(char));
        if (b->rsid[j] == NULL)
        {
            return NULL;
        }

        /* Read RSID string */
        r = fread(b->rsid[j], sizeof(char), len, fin);
        if (r != len)
        {
            io_error(fin);
            return NULL;
        }

        /* Append null-terminating character */
        b->rsid[j][len] = '\0';

        /* Read length of chromosome identifier string */
        r = fread(&len, sizeof(size_t), 1, fin);
        if (r != 1)
        {
            io_error(fin);
            return 0;
        }

        /* Allocate memory for chromosome identifier string */
        b->chr[j] = (char *)malloc((len + 1) * sizeof(char));
        if (b->chr[j] == NULL)
        {
            return NULL;
        }

        /* Read the chromosome identifier */
        r = fread(b->chr[j], sizeof(char), len, fin);
        if (r != len)
        {
            io_error(fin);
            return NULL;
        }

        /* Append null-terminating character */
        b->chr[j][len] = '\0';

        /* Read centimorgan position */
        r = fread(&(b->cm[j]), sizeof(double), 1, fin);
        if (r != 1)
        {
            io_error(fin);
            return NULL;
        }
    }

    /* Close the input file stream */
    fclose(fin);

    return b;
}

static void io_error(FILE *f)
{
    if (ferror(f))
    {
        fputs("libpbwt [ERROR]: I/O failure\n", stderr);
    }
    else if (feof(f))
    {
        fputs("libpbwt [ERROR]: truncated input file\n", stderr);
    }

    fclose(f);

    return;
}
