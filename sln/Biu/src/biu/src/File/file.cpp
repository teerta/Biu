
#include "stdafx.h"
#include "bfile.h"
#include "bfunc.h"

#ifndef _WINDOWS
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif


using namespace Biu;


#ifdef _WINDOWS

#else
void Biu::BEnsureDir(const char* path)
{
	struct stat st;
	if (0 == stat(path, &st))
	{
		return;
	}
	
	size_t cb = strlen(path);
	char* pPath = (char*)malloc(cb + 1);
	int c = '/';
	const char* psz = path;
	psz++;
	while ((psz = strchr(psz, c)))
	{
		memset(pPath, 0, cb + 1);
		strncpy(pPath, path, psz - path);
		psz++;
		if (0 == stat(pPath, &st))
		{
			continue;
		}
		
		mkdir(pPath, 0777);
	}
	
	free(pPath);
}
#endif

#ifdef _WINDOWS
long long Biu::BGetFileSize(LPCTSTR lpszFile)
{
	HANDLE hFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return -1;
	}

	LARGE_INTEGER li = { 0 };
	if (!GetFileSizeEx(hFile, &li))
	{
		return -1;
	}

	CloseHandle(hFile);

	return li.QuadPart;
}
#endif

#ifdef _WINDOWS
bool Biu::BEnsureFile(LPCTSTR lpszDir, LPTSTR lpszFile, DWORD cbFile)
{
	CString strFile;
	strFile.Format(_T("%s\\%s"), lpszDir, lpszFile);
	if (!PathFileExists(strFile))
	{
		_tcscpy_s(lpszFile, cbFile, strFile);
		return true;
	}

	CString strName,
			strExt;
	strFile = lpszFile;
	ParseFileName(strFile, strName, strExt);
	do
	{
		strFile.Format(_T("%s\\%s[%d].%s"), lpszDir, strName, BRand(1, 999999), strExt);

	} while (PathFileExists(strFile));

	_tcscpy_s(lpszFile, cbFile, strFile);

	return false;
}

#endif
