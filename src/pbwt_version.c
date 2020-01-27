#include "pbwt.h"

#define xstringy(x) #x
#define stringy(x) xstringy(x)

#ifdef VERSION
const char lpbwt_version[] = stringy(VERSION);
#endif

const char *libpbwt_version(void)
{
	return lpbwt_version;
}