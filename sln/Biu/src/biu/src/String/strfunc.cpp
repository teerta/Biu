
#include "stdafx.h"
#include "bstring.h"
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "bmarco.h"

#ifdef _WINDOWS
#include "biu.h"
#endif


using namespace Biu;
using namespace std;


#ifdef _WINDOWS


LPTSTR* Biu::PrepareArgs(LPTSTR psz, LPCTSTR sep, int* pArgc)
{
	size_t cb = (_tcslen(psz) + 1) * sizeof(TCHAR);
	LPTSTR pStr = (LPTSTR)malloc(cb);
	CHK_NULL(pStr);
	_tcscpy_s(pStr, cb, psz);
	int nArgc = 0;
	LPTSTR szTokContext = NULL;
	LPTSTR p = _tcstok_s(pStr, sep, &szTokContext);
	while (p)
	{
		nArgc++;
		p = _tcstok_s(NULL, sep, &szTokContext);
	}

	free(pStr);

	if (!nArgc)
	{
		if (pArgc)
		{
			*pArgc = 0;
			return NULL;
		}
	}

	TCHAR** pp = (TCHAR**)malloc(sizeof(TCHAR*) * (nArgc + 1));
	CHK_NULL(pp);
	pp[nArgc] = NULL;
	if (pArgc)
	{
		*pArgc = nArgc;
	}

	nArgc = 0;
	szTokContext = NULL;
	p = _tcstok_s(psz, sep, &szTokContext);
	while (p)
	{
		pp[nArgc] = p;
		p = _tcstok_s(NULL, sep, &szTokContext);
		nArgc++;
	}

	return pp;
}

LPTSTR Biu::GetDir(LPTSTR psz, LPTSTR pDir, unsigned int cbDir)
{
	assert(psz);
	assert(pDir);
	assert(cbDir);

	LPTSTR p = _tcsrchr(psz, _T('\\'));
	CHK_NULL(p);

	memset(pDir, 0, cbDir);
	_tcsncpy_s(pDir, cbDir, psz, p - psz);

	return pDir;
}


#else


char** Biu::PrepareArgs(char* psz, const char* sep, int* pArgc)
{
	char* pStr = (char*)malloc(strlen(psz) + 1);
	strcpy(pStr, psz);
	int nArgc = 0;
	char* p = strtok(pStr, sep);
	while (p)
	{
		nArgc++;        
		p = strtok(NULL, sep);
	}
	
	free(pStr);
	
	if (!nArgc)
	{
		if (pArgc)
		{
			*pArgc = 0;            
			return NULL;
		}
	}
	
	char** pp = (char**)malloc(sizeof(char*) * (nArgc + 1));
	pp[nArgc] = NULL;
	if (pArgc)
	{
		*pArgc = nArgc;
	}
	
	nArgc = 0;
	p = strtok(psz, sep);
	while (p)
	{        
		pp[nArgc] = p;        
		p = strtok(NULL, sep);
		nArgc++;
	}
	
	return pp;
}

char* Biu::GetDir(char* psz, char* pDir, unsigned int cbDir)
{
	assert(psz);
	assert(pDir);
	assert(cbDir);
	
	char* p = strrchr(psz, '/');
	CHK_NULL(p);
	
	memset(pDir, 0, cbDir);    
	memcpy(pDir, psz, p - psz);
	
	return pDir;
}


#endif
