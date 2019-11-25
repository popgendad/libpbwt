#include <stdint.h>
#include <htslib/tbx.h>
#include <htslib/vcf.h>
#include <plink_lite.h>
#include "pbwt.h"

khash_t(string) *read_popmap(const char *);

int check_popmap(const bcf_hdr_t *, const khash_t(string) *);


pbwt_t *pbwt_import_vcf(const char *infile, const char *popfile)
{
    int32_t i = 0;
    int32_t j = 0;
    int32_t nseq = 0;
    uint64_t nsites = 0;
    int32_t nsam = 0;
    const char **seq = NULL;
    tbx_t *tbx = NULL;
    bcf_hdr_t *hdr = NULL;
    htsFile *fin = NULL;
    hts_idx_t *idx = NULL;
    khash_t(string) *popdb = NULL;
    bcf1_t *rec = NULL;
    pbwt_t *b = NULL;

    /* Read the popmap file into a hash table */
    if ((popdb = read_popmap(popfile)) == NULL)
    {
        return NULL;
    }

    /* Open the input file stream */
    if ((fin = hts_open(infile, "r")) == NULL)
    {
        return NULL;
    }

    /* Read the VCF header into memory */
    if ((hdr = bcf_hdr_read(fin)) == NULL)
    {
        return NULL;
    }

    /* Load VCF index file */
    if (hts_get_format(fin)->format == vcf)
    {
        if ((tbx = tbx_index_load(infile)) == NULL)
        {
            return NULL;
        }
    }

    /* Get number of sites in VCF */
    seq = tbx ? tbx_seqnames(tbx, &nseq) : bcf_index_seqnames(idx, hdr, &nseq);
    for (i = 0; i < nseq; ++i)
    {
        uint64_t records = 0;
        uint64_t v = 0;
        hts_idx_get_stat(tbx ? tbx->idx : idx, i, &records, &v);
        nsites += records;
    }

    /* Check whether all VCF input samples are in popmap file */
    if ((nsam = check_popmap(hdr, popdb)) < 0)
    {
        return NULL;
    }

    /* Initialize memory to hold single VCF record */
    if ((rec = bcf_init()) == NULL)
    {
        bcf_hdr_destroy(hdr);
        hts_close(fin);
        return NULL;
    }

    /* Initialize pbwt structure */
    b = pbwt_init(nsites, 2 * nsam);
    if (b == NULL)
    {
        return NULL;
    }

    size_t site_counter = 0;

    /* Read through single site entry in VCF */
    while (bcf_read(fin, hdr, rec) == 0)
    {
        const char *chr;
        int32_t ngt = 0;
        int32_t *gt = NULL;
        khint_t it = 0;
 
        bcf_unpack(rec, BCF_UN_STR);
        bcf_unpack(rec, BCF_UN_INFO);
        chr = bcf_seqname(hdr, rec);
 
        b->chr[site_counter] = atoi(chr);
        b->rsid[site_counter] = strdup(rec->d.id);
        b->cm[site_counter] = rec->d.info[0].v1.f;
 
        ngt = bcf_get_genotypes(hdr, rec, &gt, &ngt) / nsam;

        /* Iterate through sample genotypes */
        for (i = 0; i < nsam; ++i)
        {
            const char *sid = hdr->samples[i];
            it = kh_get(string, popdb, sid);
            const char *pop = kh_value(popdb, it);
            int32_t *ptr = gt + i * ngt;
            const size_t len = strlen(sid);
            b->sid[2*i] = (char *)malloc(len + 3);
            b->sid[2*i+1] = (char *)malloc(len + 3);
            strcpy(b->sid[2*i], sid);
            strcat(b->sid[2*i], ".1");
            strcpy(b->sid[2*i+1], sid);
            strcat(b->sid[2*i+1], ".2");
            b->reg[2*i] = strdup(pop);
            b->reg[2*i+1] = strdup(pop);
            for (j = 0; j < ngt; ++j)
            {
                if (bcf_gt_is_missing(ptr[j]))
                {
                    break;
                }

                if (ptr[j] == bcf_int32_vector_end)
                {
                    break;
                }

                if (j % 2 == 0)
                {
                    if (bcf_gt_allele(ptr[j]) == 1)
                    {
                        b->data[TWODCORD(2*i, b->nsite, site_counter)] = '1';
                    }
                    else
                    {
                        b->data[TWODCORD(2*i, b->nsite, site_counter)] = '0';
                    }
                }
                else
                {
                    if (bcf_gt_allele(ptr[j]) == 1)
                    {
                        b->data[TWODCORD(2*i+1, b->nsite, site_counter)] = '1';
                    }
                    else
                    {
                        b->data[TWODCORD(2*i+1, b->nsite, site_counter)] = '0';
                    }
                }
            }
        }
        site_counter += 1;
    }

    /* Close up shop */
    free(seq);
    bcf_hdr_destroy(hdr);
    hts_close(fin);
    bcf_destroy(rec);
    tbx_destroy(tbx);
    hts_idx_destroy(idx);

    return b;
}


int check_popmap(const bcf_hdr_t *h, const khash_t(string) *pdb)
{
    int k = 0;
    khint_t it = 0;
    const int32_t nsam = bcf_hdr_nsamples(h);

    /* Check the sample identifiers in the VCF header against the database */
    for (k = 0; k < nsam; ++k)
    {
        const char *sid = h->samples[k];
        it = kh_get(string, pdb, sid);
        if (it == kh_end(pdb))
        {
            fprintf(stderr, "libpbwt [ERROR]: sample not in population database: %s\n", sid);
            return -1;
        }
    }
    return nsam;
}


khash_t(string) *read_popmap(const char *popfile)
{
    khash_t(string) *popdb = NULL;
    khint_t it = 0;
    char sid[256];
    char pop[256];
    int counter = 0;
    FILE *instream;

    /* Initialize sample->population hash table */
    popdb = kh_init(string);

    /* Open popmap input file stream */
    instream = fopen(popfile, "r");
    if (!instream)
    {
        return NULL;
    }

    while (!feof(instream))
    {
        int absent = 0;
        int ns = 0;

        ns = fscanf(instream, "%s\t%s\n", sid, pop);
        if (ns != 2)
        {
            fputs("libpbwt [ERROR] cannot read popmap file\n", stderr);
            return NULL;
        }
        it = kh_put(string, popdb, sid, &absent);
        if (absent)
        {
            kh_key(popdb, it) = strdup(sid);
        }
        kh_value(popdb, it) = strdup(pop);
    	counter++;
    }
    fclose(instream);
    fprintf(stderr, "libpbwt [INFO]: %d entries from %s read into pop database\n", counter, popfile);

    return popdb;
}