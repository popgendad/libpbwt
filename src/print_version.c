#include "pbwt.h"

#define xstr(s) str(s)
#define str(s) #s

#ifdef VERSION
const char version[] = xstr(VERSION);
#endif

const char *print_version(void)
{
	return version;
}