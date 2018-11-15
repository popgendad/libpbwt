#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include "pbwt.h"


int
pbwt_uncompress (pbwt_t *b)
{
    /* If data are already uncompressed */
    if (b->is_compress == FALSE)
        return 0;

    unsigned char *g;
    z_stream infstream;

    /* Allocate heap memory for uncompressed haplotype data */
    g = (unsigned char *) malloc (b->nsite * b->nsam * sizeof(unsigned char));
    if (g == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }

    /* Initialize inflate stream */
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    infstream.avail_in = b->datasize;
    infstream.next_in = (Bytef *)b->data;
    infstream.avail_out = b->nsite * b->nsam;
    infstream.next_out = (Bytef *)g;

    /* Inflate the data */
    inflateInit (&infstream);
    inflate (&infstream, Z_NO_FLUSH);
    inflateEnd (&infstream);

    /* Update pbwt data structure */
    free (b->data);
    b->data = g;
    b->datasize = infstream.total_out;
    b->is_compress = FALSE;

    return 0;
}


int
pbwt_compress (pbwt_t *b)
{
    /* Check if data are already compressed */
    if (b->is_compress == TRUE)
        return 0;

    unsigned char *f;
    z_stream defstream;

    /* Allocate heap memory for compressed haplotype data */
    f = (unsigned char *) malloc (b->nsam * b->nsite * sizeof(unsigned char));
    if (f == NULL)
    {
        perror ("libpbwt [ERROR]");
        return -1;
    }

    /* Setup the deflate stream */
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    defstream.avail_in = b->datasize;
    defstream.next_in = (Bytef *)b->data;
    defstream.avail_out = b->datasize;
    defstream.next_out = (Bytef *)f;

    /* Deflate the data */
    if (deflateInit (&defstream, Z_DEFAULT_COMPRESSION) != Z_OK)
    {
        fputs ("libpwbt [ERROR]: cannot initialize compression stream", stderr);
        return -1;
    }
    deflate (&defstream, Z_FINISH);
    deflateEnd (&defstream);

    /* Update pbwt data structure */
    free (b->data);
    b->is_compress = TRUE;
    b->data = f;
    b->datasize = defstream.total_out;

    return 0;
}
