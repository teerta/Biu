
#ifndef _BFUNC_H_
#define _BFUNC_H_


#include "biu.h"


#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _WINDOWS
#include <ATLComTime.h>
#endif

#include "btype.h"


using namespace std;

#ifdef _WINDOWS
using namespace ATL;
#endif


namespace Biu {


	static inline 
	double 
	logbase(double a, double base)
	{
		return log(a) / log(base);
	}

	static inline 
	double 
	round(double a)
	{
		return floor(a + 0.5);
	}

	static inline 
	double 
	trunc(double a)
	{
		return a >= 0 ? floor(a) : ceil(a);
	}


	EXTERN_C
	BIU_API
	int 
	BRand(int nMin, int nMax);


	#ifdef _WINDOWS

	EXTERN_C
	BIU_API
	void
	GetCurrentOleTimeUTC(COleDateTime& t);

	EXTERN_C
	BIU_API
	void
	UTCOleTimeToLocalOleTime(COleDateTime& t);

	EXTERN_C
	BIU_API
	time_t 
	FileTimeToTime(const FILETIME& ft);

	EXTERN_C
	BIU_API
	void 
	TimeToFileTime(time_t t, FILETIME& ft);

	EXTERN_C
	BIU_API
	time_t 
	OleTimeToTime(const COleDateTime& t, bool bConvertToUTC = false);

	EXTERN_C
	BIU_API
	void 
	TimeToOleTime(time_t t, COleDateTime& olet);

	EXTERN_C
	BIU_API
	void
	TimeToOleTimeUTC(time_t t, COleDateTime& olet);

	EXTERN_C
	BIU_API
	TIMEVAR 
	OleTimeToTimeVar(const COleDateTime& t, bool bIsTime64 = true);

	EXTERN_C
	BIU_API
	void 
	TimeVarToOleTime(TIMEVAR tv, COleDateTime& t, bool bIsTime64 = true);

	#endif // _WINDOWS


}


#endif // _BFUNC_H_
