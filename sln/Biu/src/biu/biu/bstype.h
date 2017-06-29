
#ifndef _BSTYPE_H_
#define _BSTYPE_H_


#include "biu.h"


namespace Biu {
	
	
	#define BSBR(s) if(!(s)) { return false; }


	typedef enum tagX_S_Type : char
	{
			X_S_N1 = 1,
			X_S_N2 = 2,
			X_S_N4 = 3,
			X_S_N8 = 4,
			X_S_V = 100
	} X_S_Type, *LPX_S_Type;

	
	// Functions

	EXTERN_C
	BIU_API
	unsigned long long 
	htonll(unsigned long long value);

	inline unsigned long long ntohll(unsigned long long value) { return htonll(value); }

	void htonwchar(wchar_t* pval, unsigned int cbVal);

	inline void ntohwchar(wchar_t* pval, unsigned int cbVal) { return htonwchar(pval, cbVal); }


	unsigned long long pack754(long double f, unsigned bits, unsigned expbits);
	long double unpack754(unsigned long long i, unsigned bits, unsigned expbits);

	#define pack754_32(f) (pack754((f), 32, 8))
	#define pack754_64(f) (pack754((f), 64, 11))
	#define unpack754_32(i) (unpack754((i), 32, 8))
	#define unpack754_64(i) (unpack754((i), 64, 11))
	
	
}


#endif // _BSTYPE_H_
