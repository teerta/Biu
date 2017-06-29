
#include "stdafx.h"
#include "bfunc.h"


using namespace Biu;


#ifdef _WINDOWS

void
Biu::
GetCurrentOleTimeUTC(COleDateTime& t)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	t = st;
}

void
Biu::
UTCOleTimeToLocalOleTime(COleDateTime& t)
{
	SYSTEMTIME stUTC = { 0 };
	t.GetAsSystemTime(stUTC);

	SYSTEMTIME st = { 0 };
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &st);
	t = st;
}

void
Biu::
TimeToOleTimeUTC(time_t t, COleDateTime& olet)
{
	FILETIME ft = { 0 };
	TimeToFileTime(t, ft);

	SYSTEMTIME st = { 0 };
	FileTimeToSystemTime(&ft, &st);

	olet = st;
}

time_t 
Biu::
FileTimeToTime(const FILETIME& ft)
{
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	return ull.QuadPart / 10000000ULL - 11644473600ULL;
}

void 
Biu::
TimeToFileTime(time_t t, FILETIME& ft)
{
	ULARGE_INTEGER ull;
	ull.QuadPart = (t + 11644473600ULL) * 10000000ULL;
	ft.dwLowDateTime = ull.LowPart;
	ft.dwHighDateTime = ull.HighPart;
}

time_t 
Biu::
OleTimeToTime(const COleDateTime& t, bool bConvertToUTC)
{
	SYSTEMTIME	st = { 0 },
		stUTC = { 0 };
	LPSYSTEMTIME lpst = &st;
	t.GetAsSystemTime(st);

	if (bConvertToUTC)
	{
		TzSpecificLocalTimeToSystemTime(NULL, &st, &stUTC);
		lpst = &stUTC;
	}

	FILETIME ft = { 0 };
	SystemTimeToFileTime(lpst, &ft);

	return FileTimeToTime(ft);
}

void 
Biu::
TimeToOleTime(time_t t, COleDateTime& olet)
{
	FILETIME ft;
	TimeToFileTime(t, ft);

	olet = ft;
}

TIMEVAR 
Biu::
OleTimeToTimeVar(const COleDateTime& t, bool bIsTime64)
{
	SYSTEMTIME st = { 0 };
	t.GetAsSystemTime(st);

	FILETIME ft = { 0 };
	SystemTimeToFileTime(&st, &ft);

	TIMEVAR tv;
	if (bIsTime64)
	{
		tv.llt = FileTimeToTime(ft);
	}
	else
	{
		tv.lt = (long)FileTimeToTime(ft);
	}

	return tv;
}

void 
Biu::
TimeVarToOleTime(TIMEVAR tv, COleDateTime& t, bool bIsTime64)
{
	FILETIME ft;
	TimeToFileTime(bIsTime64 ? tv.llt : (long long)tv.lt, ft);

	t = ft;
}

#endif // _WINDOWS
