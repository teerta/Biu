
#include "stdafx.h"
#include "bstype.h"
#include <stdlib.h>

#if defined(__linux__) || defined(__APPLE__)
#include <netinet/in.h>
#endif // __linux__


using namespace Biu;


#define TYP_INIT 0 
#define TYP_SMLE 1 
#define TYP_BIGE 2 


unsigned long long Biu::htonll(unsigned long long value)
{
	long long num = 42;
	if (*reinterpret_cast<const char*>(&num) == num)
	{
	// little

		const unsigned long high_part = htonl(static_cast<unsigned long>(value >> 32));
		const unsigned long low_part = htonl(static_cast<unsigned long>(value & 0xFFFFFFFFLL));

		return (static_cast<unsigned long long>(low_part) << 32) | high_part;
	} else
	{
	// big

		return value;
	}
}


//unsigned long long htonll(unsigned long long src)
//{ 
//	static int typ = TYP_INIT; 
//	unsigned char c; 	
//	union 
//	{ 
//		unsigned long long ull; 
//		unsigned char c[8]; 
//	} x;
//
//	if (typ == TYP_INIT) 
//	{ 
//		x.ull = 0x01; 
//		typ = (x.c[7] == 0x01ULL) ? TYP_BIGE : TYP_SMLE; 
//	} 
//
//	if (typ == TYP_BIGE) 
//	{
//		return src; 
//	}
//	
//	x.ull = src; 
//	
//	c = x.c[0]; x.c[0] = x.c[7]; x.c[7] = c; 
//	c = x.c[1]; x.c[1] = x.c[6]; x.c[6] = c; 
//	c = x.c[2]; x.c[2] = x.c[5]; x.c[5] = c; 
//	c = x.c[3]; x.c[3] = x.c[4]; x.c[4] = c; 
//	
//	return x.ull; 
//}

void Biu::htonwchar(wchar_t* pval, unsigned int cbVal)
{
	const size_t cbWchar = sizeof(wchar_t);
	static const int num = 42;

	if (*reinterpret_cast<const char*>(&num) == num)
	{
		// little

		char* p = (char*)pval;	
		char pt = NULL;
		unsigned int ul = 0;
		for (unsigned int i = 0; i < cbVal; i++)
		{
			ul = i * cbWchar;
			pt = p[ul];
			p[ul] = p[ul + 1];
			p[ul + 1] = pt;
		}
	}
	//else
	//{
	//	// big
	//}
}

unsigned long long Biu::pack754(long double f, unsigned bits, unsigned expbits)
{
	long double fnorm;
	int shift;
	long long sign, exp, significand;
	unsigned significandbits = bits - expbits - 1; // -1 for sign bit

	if (f == 0.0) return 0; // get this special case out of the way

	// check sign and begin normalization
	if (f < 0) { sign = 1; fnorm = -f; }
	else { sign = 0; fnorm = f; }

	// get the normalized form of f and track the exponent
	shift = 0;
	while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
	while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
	fnorm = fnorm - 1.0;

	// calculate the binary form (non-float) of the significand data
	significand = fnorm * ((1LL<<significandbits) + 0.5f);

	// get the biased exponent
	exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

	// return the final answer
	return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double Biu::unpack754(unsigned long long i, unsigned bits, unsigned expbits)
{
	long double result;
	long long shift;
	unsigned bias;
	unsigned significandbits = bits - expbits - 1; // -1 for sign bit

	if (i == 0) return 0.0;

	// pull the significand
	result = (i&((1LL<<significandbits)-1)); // mask
	result /= (1LL<<significandbits); // convert back to float
	result += 1.0f; // add the one back on

	// deal with the exponent
	bias = (1<<(expbits-1)) - 1;
	shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
	while(shift > 0) { result *= 2.0; shift--; }
	while(shift < 0) { result /= 2.0; shift++; }

	// sign it
	result *= (i>>(bits-1))&1? -1.0: 1.0;

	return result;
}
