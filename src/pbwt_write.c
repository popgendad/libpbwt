#include <stdio.h>
#include <string.h>
#include "pbwt.h"

static void io_error (FILE *);

int
pbwt_write (const char *outfile, pbwt_t *b)
{
    /* If data aren't compressed, then compress */
    if (b->is_compress == FALSE)
        pbwt_compress (b);

    size_t i;
    size_t j;
    size_t r;
    FILE *fout;

    /* Open the binary output file stream */
    fout = fopen (outfile, "wb");
    if (!fout)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }

    /* Write the data to the output file */

    /* Write the number of sites */
    if (fwrite ((const void *)&(b->nsite), sizeof(size_t), 1, fout) != 1)
    {
        io_error (fout);
        return -1;
    }

    /* Write the number of samples */
    r = fwrite ((const void *)&(b->nsam), sizeof(size_t), 1, fout);
    if (r != 1)
    {
        io_error (fout);
        return -1;
    }

    /* Write the size of the haplotype data */
    r = fwrite ((const void *)&(b->datasize), sizeof(size_t), 1, fout);
    if (r != 1)
    {
        io_error (fout);
        return -1;
    }

    /* Write the haplotype data */
    r = fwrite ((const void *)b->data, sizeof(unsigned char), b->datasize, fout);
    if (r != b->datasize)
    {
        io_error (fout);
        return -1;
    }

    /* Write the prefix array */
    r = fwrite ((const void *)b->ppa, sizeof(size_t), b->nsam, fout);
    if (r != b->nsam)
    {
        io_error (fout);
        return -1;
    }

    /* Write the divergence array */
    r = fwrite ((const void *)b->div, sizeof(size_t), b->nsam, fout);
    if (r != b->nsam)
    {
        io_error (fout);
        return -1;
    }

    /* Write sample info */
    for (i = 0; i < b->nsam; ++i)
    {
        size_t len;

        /* Write the size of the sample identifier string */
        len = strlen (b->sid[i]);
        r = fwrite ((const void *)&len, sizeof(size_t), 1, fout);
        if (r != 1)
        {
            io_error (fout);
            return -1;
        }

        /* Write the sample identifier string */
        r = fwrite (b->sid[i], sizeof(char), len, fout);
        if (r != len)
        {
            io_error (fout);
            return -1;
        }

        /* Write the length of the region string */
        len = strlen (b->reg[i]);
        r = fwrite ((const void *)&len, sizeof(size_t), 1, fout);
        if (r != 1)
        {
            io_error (fout);
            return -1;
        }

        /* Write the region string */
        r = fwrite (b->reg[i], sizeof(char), len, fout);
        if (r != len)
        {
            io_error (fout);
            return -1;
        }
    }

    for (j = 0; j < b->nsite; ++j)
    {
        size_t len;

        /* Write the size of the RSID string */
        len = strlen (b->rsid[j]);
        r = fwrite ((const void *)&len, sizeof(size_t), 1, fout);
        if (r != 1)
        {
            io_error (fout);
            return -1;
        }

        /* Write the RSID string */
        r = fwrite (b->rsid[j], sizeof(char), len, fout);
        if (r != len)
        {
            io_error (fout);
            return -1;
        }

        /* Write the chromosome identifier */
        r = fwrite ((const void *)&(b->chr[j]), sizeof(int), 1, fout);
        if (r != 1)
        {
            io_error (fout);
            return -1;
        }

        /* Write the centimorgan position */
        r = fwrite ((const void *)&(b->cm[j]), sizeof(double), 1, fout);
        if (r != 1)
        {
            io_error (fout);
            return -1;
        }
    }

    /* Close output file stream */
    fclose (fout);

    return 0;
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
