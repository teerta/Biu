
#ifndef _BFILE_H_
#define _BFILE_H_


#include "biu.h"
#ifdef _WINDOWS
#include <atlstr.h>

using namespace ATL;
#endif


namespace Biu {


#ifdef __cplusplus
	extern "C" {
#endif


#ifdef _WINDOWS
	long long BIU_API BGetFileSize(LPCTSTR lpszFile);
	bool BIU_API BEnsureFile(LPCTSTR lpszDir, LPTSTR lpszFile, DWORD cbFile);
#else
void BEnsureDir(const char* path);
#endif


#ifdef __cplusplus
	}
#endif


	inline void TailSlash(CString& str)
	{
		if (_T('\\') != str.Right(1))
		{
			str += _T('\\');
		}
	}

	inline void RetriveFileName(CString& strAppFullPath)
	{
		strAppFullPath = strAppFullPath.Right(strAppFullPath.GetLength() - strAppFullPath.ReverseFind(_T('\\')) - 1);
	}

	inline void RetriveAppFileName(CString& strAppFullPath)
	{
		strAppFullPath = strAppFullPath.Right(strAppFullPath.GetLength() - strAppFullPath.ReverseFind(_T('\\')) - 1);
		strAppFullPath = strAppFullPath.Left(strAppFullPath.Find(_T(".exe")) + 4);
	}

	inline void RetriveAppFileName2(CString& strAppFullPath)
	{
		strAppFullPath = strAppFullPath.Left(strAppFullPath.Find(_T(".exe")) + 4);
		RetriveAppFileName(strAppFullPath);
	}

	inline void RetriveAppPath(CString& strAppFullPath)
	{
		strAppFullPath = strAppFullPath.Left(strAppFullPath.ReverseFind(_T('\\')));
	}

	inline void RetriveFileExt(CString& str)
	{
		str = str.Right(str.GetLength() - str.ReverseFind(_T('.')) - 1);
	}

	inline void ParseFilePath(CString& strPath, CString& strDir, CString& strName, CString& strExt)
	{
		int nPos1 = strPath.ReverseFind(_T('\\'));
		strDir = strPath.Right(strPath.GetLength() - nPos1 - 1);

		int nPos2 = strDir.ReverseFind(_T('.'));
		strExt = strDir.Right(strDir.GetLength() - nPos2 - 1);
		strName = strDir.Left(nPos2);

		strDir = strPath.Left(nPos1);
	}

	inline void ParseFileName(CString& strFile, CString& strName, CString& strExt)
	{
		int nPos = strFile.ReverseFind(_T('.'));
		strExt = strFile.Right(strFile.GetLength() - nPos - 1);
		strName = strFile.Left(nPos);
	}


}


#endif // _BFILE_H_
