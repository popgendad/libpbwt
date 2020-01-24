#include <stdio.h>
#include "pbwt.h"

static void io_error(FILE *);

int pbwt_write(const char *outfile, pbwt_t *b)
{
    /* If data aren't compressed, then compress */
    if (b->is_compress == FALSE)
    {
        pbwt_compress(b);
    }

    size_t i = 0;
    size_t j = 0;
    size_t r = 0;
    FILE *fout = NULL;

    /* Open the binary output file stream */
    fout = fopen(outfile, "wb");
    if (!fout)
    {
        return -1;
    }

    /* Write the data to the output file */

    /* Write the number of sites */
    if (fwrite((const void *)&(b->nsite), sizeof(size_t), 1, fout) != 1)
    {
        io_error(fout);
        return -1;
    }

    /* Write the number of samples */
    r = fwrite((const void *)&(b->nsam), sizeof(size_t), 1, fout);
    if (r != 1)
    {
        io_error(fout);
        return -1;
    }

    /* Write the size of the haplotype data */
    r = fwrite((const void *)&(b->datasize), sizeof(size_t), 1, fout);
    if (r != 1)
    {
        io_error(fout);
        return -1;
    }

    /* Write the haplotype data */
    r = fwrite((const void *)b->data, sizeof(unsigned char), b->datasize, fout);
    if (r != b->datasize)
    {
        io_error(fout);
        return -1;
    }

    /* Write sample-based information */
    for (i = 0; i < b->nsam; ++i)
    {
        size_t len = 0;

        /* Write the length of the sample identifier string */
        len = strlen(b->sid[i]);
        r = fwrite((const void *)&len, sizeof(size_t), 1, fout);
        if (r != 1)
        {
            io_error(fout);
            return -1;
        }

        /* Write the sample identifier string */
        r = fwrite(b->sid[i], sizeof(char), len, fout);
        if (r != len)
        {
            io_error(fout);
            return -1;
        }

        /* Write the length of the region string */
        len = strlen(b->reg[i]);
        r = fwrite((const void *)&len, sizeof(size_t), 1, fout);
        if (r != 1)
        {
            io_error(fout);
            return -1;
        }

        /* Write the region string */
        r = fwrite(b->reg[i], sizeof(char), len, fout);
        if (r != len)
        {
            io_error(fout);
            return -1;
        }
    }

    /* Write site-based information */
    for (j = 0; j < b->nsite; ++j)
    {
        size_t len = 0;

        /* Write the size of the RSID string */
        len = strlen(b->rsid[j]);
        r = fwrite((const void *)&len, sizeof(size_t), 1, fout);
        if (r != 1)
        {
            io_error(fout);
            return -1;
        }

        /* Write the RSID string */
        r = fwrite(b->rsid[j], sizeof(char), len, fout);
        if (r != len)
        {
            io_error(fout);
            return -1;
        }

        /* Write the size of the chromosome identifier string */
        len = strlen(b->chr[j]);
        r = fwrite((const void *)&len, sizeof(size_t), 1, fout);
        if (r != 1)
        {
            io_error(fout);
            return -1;
        }

        /* Write the chromosome identifier */
        r = fwrite(b->chr[j], sizeof(char), len, fout);
        if (r != len)
        {
            io_error(fout);
            return -1;
        }

        /* Write the centimorgan position */
        r = fwrite((const void *)&(b->cm[j]), sizeof(double), 1, fout);
        if (r != 1)
        {
            io_error(fout);
            return -1;
        }
    }

    /* Close output file stream */
    fclose(fout);

    return 0;
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
