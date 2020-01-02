#include <stdint.h>
#include <htslib/tbx.h>
#include <htslib/vcf.h>
#include "pbwt.h"

#define MAX_STRLEN 256

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

    /* Fill in sample information */
    khint_t it = 0;
    for (i = 0; i < nsam; ++i)
    {
        const char *sid = hdr->samples[i];
        it = kh_get(string, popdb, sid);
        const char *pop = kh_value(popdb, it);
        const size_t len = strlen(sid);
        b->sid[2*i] = (char *)malloc(len + 3);
        b->sid[2*i+1] = (char *)malloc(len + 3);
        strcpy(b->sid[2*i], sid);
        strcat(b->sid[2*i], ".1");
        strcpy(b->sid[2*i+1], sid);
        strcat(b->sid[2*i+1], ".2");
        b->reg[2*i] = strdup(pop);
        b->reg[2*i+1] = strdup(pop);
    }

    size_t site_counter = 0;
    const char *tag = "CM";
    int last_size = (int)sizeof(float);

    /* Read through single site entry in VCF */
    while (bcf_read(fin, hdr, rec) == 0)
    {
        int k = last_size / sizeof(float);
        int ret = 0;
        void *tmp = NULL;
        float *p = NULL;
        const char *chr = NULL;
        int32_t ngt = 0;
        int32_t *gt = NULL;

        bcf_unpack(rec, BCF_UN_STR);
        bcf_unpack(rec, BCF_UN_INFO);
        chr = bcf_seqname(hdr, rec);
        b->chr[site_counter] = strdup(chr);
        b->rsid[site_counter] = strdup(rec->d.id);

        /* Safe way to get INFO/CM tag */
        ret = bcf_get_info_values(hdr, rec, tag, &tmp, &k, BCF_HT_REAL);
        if (ret <= 0)
        {
            return NULL;
        }
        p = (float *)tmp;
        b->cm[site_counter] = (double)(*p);
        last_size = k * sizeof(float);

        ngt = bcf_get_genotypes(hdr, rec, &gt, &ngt) / nsam;

        /* Iterate through sample genotypes */
        for (i = 0; i < nsam; ++i)
        {
            int32_t *ptr = gt + i * ngt;
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
        site_counter++;
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
            return -1;
        }
    }

    return nsam;
}

khash_t(string) *read_popmap(const char *popfile)
{
    khash_t(string) *popdb = NULL;
    khint_t it = 0;
    char sid[MAX_STRLEN];
    char pop[MAX_STRLEN];
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

    /* Close input file stream */
    fclose(instream);

    return popdb;
}
