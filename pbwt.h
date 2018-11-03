#ifndef PBWT_H
#define PBWT_H

/* Define values */
#ifndef BOOL_DEFINED
#define BOOL_DEFINED
typedef char BOOL;
#define TRUE 1
#define FALSE 0
#endif

#define ARRAY_MAGIC 8918274

/* Basic data structures */
typedef struct ArrayStruct
{
    int magic;
    char *base;         /* char* since need to do pointer arithmetic in bytes */
    long dim;           /* length of alloc'ed space */
    int size;
    long max;           /* largest element accessed via array() */
    int id;             /* unique identifier */
} *Array;

typedef struct
{
  char **names;
  int *table;
  int max;		          /* current number of entries */
  int dim;
  int size;             /* 2^dim = size of tables */
} DICT;

typedef struct PBWTstruct
{
  int nsite; 	          /* number of sites */
  int nsam;             /* number of samples */
  char *chr;	  		    /* chromosome name */
  Array sites;			    /* array of Site */
  Array samples;		    /* array of int index into global samples */
  Array yz;			        /* compressed PBWT array of uchar */
  int *aFstart, *aFend;	/* start and end a[] index arrays for forwards cursor */
  Array zz;			        /* compressed reverse PBWT array of uchar */
  int *aRstart, *aRend; /* start and end a[] index arrays for reverse cursor */
  /* NB aRend is the lexicographic sort order for the data, and aFend the reverse lex order */
  /* probably it is optimal to have aFstart == aRend and vice versa: to be done */
  Array zMissing;		    /* compressed array of uchar - natural not sort order */
  Array missingOffset;	/* of long, site index into zMissing, 0 if no missing data at site */
  Array zDosage;		    /* run-length compressed array of uchar in local sort order */
  Array dosageOffset;	  /* of long, site index into zDosage, 0 if no dosage data */
  BOOL isRefFreq;		    /* some flags for the whole VCF */
  BOOL isUnphased;
} PBWT;

typedef struct SiteStruct
{
  int x;			          /* position on chromosome */
  int varD;			        /* index in variationDict */
  double freq;			    /* frequency */
  double refFreq;		    /* frequency from reference used for last phasing or imputation */
  double imputeInfo;		/* estimated r^2 from imputation */
} Site;

typedef struct SampleStruct
{
  int nameD;			      /* index in sampleDict */
  int father;			      /* index into samples */
  int mother;			      /* index into samples */
  int popD;			        /* index in populationDict */
  BOOL isMale;			    /* treat X chromosome as haploid */
  BOOL isFemale;		    /* treat X chromosome as diploid and ignore Y */
} Sample;

/* data structure for moving forwards - doesn't know PBWT */
typedef struct
{
  int M;
  Array z;			        /* packed byte array; if zero y needs loading from elsewhere */
  long n;			          /* position in packed byte array */
  BOOL isBlockEnd;		  /* TRUE if n is at end of next block, FALSE if at start */
  unsigned char *y;     /* current value in sort order */
  int c;			          /* number of 0s in y */
  int *a;			          /* index back to original order */
  int *d;			          /* location of last match */
  int *u;			          /* number of 0s up to and including this position */
  int *b;			          /* for local operations - no long term meaning */
  int *e;			          /* for local operations - no long term meaning */
  long nBlockStart;		  /* u->n at start of block encoding current u->y */
} PbwtCursor;

extern BOOL isCheck;		/* when TRUE carry out various checks */
extern BOOL isStats;		/* when TRUE report stats in various places */
extern DICT *variationDict;	/* "xxx|yyy" where variation is from xxx to yyy in VCF */

/* Function prototypes */

/* Functions in pbwt.c */
int pbwtInit (void);
PBWT *pbwtCreate (int M, int N); /* OK to have N == 0 and set p->N later if not known now */
int pbwtDestroy (PBWT *p);

PBWT *pbwtSubSites (PBWT *pOld, double fmin, double frac);
PBWT *pbwtSubRange (PBWT *pOld, int start, int end);
void pbwtBuildReverse (PBWT *p);
unsigned char **pbwtHaplotypes (PBWT *p);
PBWT *pbwtSelectSites (PBWT *pOld, Array sites, BOOL isKeepOld);
PBWT *pbwtSelectSitesFillMissing (PBWT *pOld, Array sites, BOOL isKeepOld);
PBWT *pbwtRemoveSites (PBWT *pOld, Array sites, BOOL isKeepOld);

#endif