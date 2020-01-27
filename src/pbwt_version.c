#include "pbwt.h"

#define xstringy(x) #x
#define stringy(x) xstringy(x)

#ifdef VERSION
const char version[] = stringy(VERSION);
#endif

const char *pbwt_version(void)
{
	return version;
}