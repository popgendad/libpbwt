#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <plink.h>
#include "pbwt.h"

int
main (int argc, char **argv)
{
	int has_reg = 1;
	int is_phased = 1;
	int v = 0;
	char *instub;
	plink_t *p;
	pbwt_t *b;

	instub = strdup(argv[1]);

	/* Initialize plink data structure */
	p = read_plink(instub, has_reg, is_phased);
	if (p == NULL)
		return EXIT_FAILURE;

	/* Initialize pbwt structure */
	b = pbwt_init(p->nsnp, 2* p->nsam);

    size_t i = 0;
    char qreg[] = "Beringia";

    /* Iterate through all samples in the fam/reg */
    for (i = 0; i < p->nsam; ++i)
    {
        //char *res = query_reg(p->reg, p->fam[i].iid);
        //if (res != NULL) /* && (strcmp(qreg , res) == 0))*/
        //{
        	memcpy(b->data[2*i], hap2uchar(p, i, 0), b->nsite * sizeof(unsigned char));
        	memcpy(b->data[2*i+1], hap2uchar(p, i, 1), b->nsite * sizeof(unsigned char));
        //}
    }

	/* Build the prefix and divergence arrays */
	v = build_prefix_array(b);

	/* Print data structure */
	v = pbwt_print(b);

	v = pbwt_destroy(b);

	return v;
}
