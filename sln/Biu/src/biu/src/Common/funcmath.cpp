
#include "stdafx.h"
#include "bfunc.h"


#include <limits.h>


using namespace Biu;


int 
Biu::
BRand(int nMin, int nMax)
{
#ifdef _WINDOWS
	unsigned int nVal;
	rand_s(&nVal);

	return nMin +(int) round((double)nVal / ((double)UINT_MAX + 1) * (nMax - nMin));
#else
	time_t t;
	time(&t);
	srand48((long)t);
	
	return nMin +(int) round((double)lrand48() / ((double)UINT_MAX + 1) * (nMax - nMin));
#endif
}
