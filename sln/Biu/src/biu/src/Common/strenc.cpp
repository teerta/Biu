
#include "stdafx.h"
#include "bstrenc.h"


#ifdef _WINDOWS


#include "bmarco.h"


using namespace Biu;


// BEncoder

BEncoder::BEncoder()
{
}

BEncoder::~BEncoder()
{
}

int BEncoder::ConvertToUTF8(LPCWSTR lpsz, LPSTR* ppszUTF8)
{
	CHK_N1(lpsz);

	int cb = 0;
	if (wcscmp(L"", lpsz))
	{
		cb = WideCharToMultiByte(CP_UTF8, NULL, lpsz, wcslen(lpsz), NULL, 0, NULL, NULL);
		CHK_N1(cb);
	}	

	if (*ppszUTF8)
	{
		delete[] *ppszUTF8;
		*ppszUTF8 = 0;
	}

	*ppszUTF8 = new char[cb + 1];
	if (cb)
	{
		WideCharToMultiByte(CP_UTF8, NULL, lpsz, wcslen(lpsz), *ppszUTF8, cb, NULL, NULL);
	}
	(*ppszUTF8)[cb] = L'\0';

	return cb;
}

int BEncoder::ConvertToUTF8(CStringW& strWide, LPSTR* ppszUTF8)
{
	int cb = WideCharToMultiByte(CP_UTF8, NULL, strWide, strWide.GetLength(), NULL, 0, NULL, NULL);
	CHK_N1(cb);

	if (*ppszUTF8)
	{
		delete[] *ppszUTF8;
		*ppszUTF8 = 0;
	}

	*ppszUTF8 = new char[cb + 1];
	WideCharToMultiByte(CP_UTF8, NULL, strWide, strWide.GetLength(), *ppszUTF8, cb, NULL, NULL);
	(*ppszUTF8)[cb] = L'\0';

	return cb;
}

int BEncoder::ConvertToUnicode(LPCSTR lpszUTF8, LPWSTR* ppsz)
{
	CHK_N1(lpszUTF8);

	int cb = MultiByteToWideChar(CP_UTF8, NULL, lpszUTF8, strlen(lpszUTF8), NULL, 0);
	CHK_N1(cb);

	if (*ppsz)
	{
		delete[] *ppsz;
		*ppsz = 0;
	}

	*ppsz = new wchar_t[cb + 1];

	cb = MultiByteToWideChar(CP_UTF8, NULL, lpszUTF8, strlen(lpszUTF8), *ppsz, cb + 1);
	(*ppsz)[cb] = L'\0';

	return cb;
}

int BEncoder::ConvertToUnicode(LPCSTR lpszUTF8, CStringW& strWide)
{
	CHK_N1(lpszUTF8);

	int cb = MultiByteToWideChar(CP_UTF8, NULL, lpszUTF8, strlen(lpszUTF8), NULL, 0);
	CHK_N1(cb);

	wchar_t* pszWide = strWide.GetBuffer(cb + 1);

	cb = MultiByteToWideChar(CP_UTF8, NULL, lpszUTF8, strlen(lpszUTF8), pszWide, cb + 1);
	pszWide[cb] = L'\0';
	strWide.ReleaseBuffer();

	return cb;
}

#endif // _WINDOWS
