#include "pbwt.h"

#define str(s) #s
#define xstr(s) str(s)

#ifdef VERSION
const char version[] = xstr(VERSION);
#endif

const char *pbwt_version(void)
{
	return version;
}