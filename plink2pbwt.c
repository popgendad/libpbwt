#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <plink.h>
#include "pbwt.h"

int print_usage(const char *);

extern int opterr, optopt, optind;
extern char *optarg;

int
main (int argc, char **argv)
{
	int c;
	int has_reg = 0;
	int is_phased = 0;
	int v = 0;
	size_t i = 0;
	char *query;
	char *instub;
	plink_t *p;
	pbwt_t *b;

	/* Get command line options */
	opterr = 0;
	while ((c = getopt (argc, argv, "rphq:")) != -1)
	{
		switch(c)
		{
			case 'r':
				has_reg = 1;
				break;
			case 'p':
				is_phased = 1;
				break;
			case 'q':
				query = strdup (optarg);
				break;
			case 'h':
				print_usage (NULL);
				return EXIT_SUCCESS;
			case '?':
				fprintf(stderr, "plink2pbwt [ERROR]: unknown option \"-%c\".\n", optopt);
				return EXIT_SUCCESS;
			default:
				print_usage(NULL);
				return EXIT_SUCCESS;
		}
	}

	/* Parse non-optioned arguments */
	if (optind != argc - 1)
	{
		print_usage("plink2pbwt [ERROR]: need PLINK input stub as mandatory argument");
		return EXIT_FAILURE;
	}
	else
		instub = strdup (argv[optind]);

	/* Initialize plink data structure */
	p = read_plink(instub, has_reg, is_phased);
	if (p == NULL)
		return EXIT_FAILURE;

	/* Initialize pbwt structure */
	b = pbwt_init(p->nsnp, 2* p->nsam);

    /* Iterate through all samples in the fam/reg */
    for (i = 0; i < p->nsam; ++i)
    {
        	memcpy(b->data[2*i], hap2uchar(p, i, 0), b->nsite * sizeof(unsigned char));
        	memcpy(b->data[2*i+1], hap2uchar(p, i, 1), b->nsite * sizeof(unsigned char));
        	b->sid[2*i] = strdup (p->fam[i].iid);
        	b->sid[2*i+1] = strdup (p->fam[i].iid);
        	b->reg[2*i] = strdup(p->reg[i].reg);
        	b->reg[2*i+1] = strdup (p->reg[i].reg);
    }

	/* Build the prefix and divergence arrays */
	v = build_prefix_array(b);

	/* Print data structure */
	v = pbwt_print(b);

	/* Deallocate memory for bpwt structure */
	v = pbwt_destroy(b);

	return v;
}

int
print_usage (const char *msg)
{
    puts("Usage: plink2pbwt [OPTION]... [PLINK STUB]");
    puts("Test of libpbwt API\n");
    if (msg)
        printf("%s\n\n", msg);
    puts("Options:");
    puts("  -p      Input data are phased");
    puts("  -r      Input PLINK stub includes a REG file");
    puts("  -q STR  Use only this region/population (requires -r switch");
    puts("  -h      Display this help message and exit");
    putchar('\n');  

    return 0;
}
