
#include "stdafx.h"
#include "bpeerdata.h"
#include "bbfunc.h"

#include <cassert>
#include <algorithm>


using namespace std;
using namespace Biu;


// BDataInfo

BDataInfo::BDataInfo()
	: m_cb(0)
	, m_ullOrigin(0)
	, m_ullFinished(0)
	, m_nID(0)
	, m_hFile(INVALID_HANDLE_VALUE)
	, m_hFileMapping(NULL)
	, m_lpv(NULL)
	, m_pHeader(NULL)
#ifdef _WINDOWS
	, m_bTransmitting(0L)
#endif
{
}

BDataInfo::~BDataInfo()
{
	CloseFile();
}

bool BDataInfo::PrepareForRecv(int nID, unsigned long long cb, LPCTSTR lpszFile)
{
	m_cb = cb;
	m_bDir = false;
	m_strFile = lpszFile;
	m_hFile = ::CreateFile(lpszFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return false;
	}

	m_nID = nID;
	LARGE_INTEGER li = { 0 };
	li.QuadPart = m_cb;
	if (!SetFilePointerEx(m_hFile, li, NULL, FILE_BEGIN))
	{
		SAFE_CLOSE_HANDLE_INVALID(m_hFile);
		return false;
	}

	if (!SetEndOfFile(m_hFile))
	{
		SAFE_CLOSE_HANDLE_INVALID(m_hFile);
		return false;
	}

	li.QuadPart = 0;
	if (!SetFilePointerEx(m_hFile, li, NULL, FILE_BEGIN))
	{
		SAFE_CLOSE_HANDLE_INVALID(m_hFile);
		return false;
	}

	return MapFile();
}

bool BDataInfo::PrepareForSend(int nID, LPCTSTR lpszFile)
{
	if (!PathFileExists(lpszFile))
	{
		return false;
	}

	bool bRet = false;
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		m_bDir = true;
		m_strFile = lpszFile;
		m_hFile = CreateFile(lpszFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		bRet = INVALID_HANDLE_VALUE != m_hFile;
		if (bRet)
		{
			m_nID = nID;
			LARGE_INTEGER cb;
			if (GetFileSizeEx(m_hFile, &cb))
			{
				m_cb = cb.QuadPart;
			}
			else
			{
				CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
				bRet = false;
			}
		}
	}

	return bRet ? MapFile() : false;
}

void BDataInfo::CloseFile()
{
	SAFE_DELETE(m_pHeader);
	SAFE_DELETE(m_pParam);

	if (m_hFileMapping)
	{
		UnmapViewOfFile(m_lpv);
		CloseHandle(m_hFileMapping);
		m_lpv = NULL;
	}

	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

void BDataInfo::GetFile(CString& str)
{
	str = m_strFile;
}

unsigned long long BDataInfo::Read(
	void* pBuff, 
	unsigned long long cb, 
	bool bUpdateFinished,
	bool bNewOrigin, 
	unsigned long long ullNewPos)
{
	unsigned long long ullPos = bNewOrigin ? ullNewPos : m_ullOrigin + m_ullFinished;
	unsigned long long ullSize = min<unsigned long long>(cb, m_cb - m_ullFinished);
	memcpy(pBuff, (char*)m_lpv + ullPos, ullSize);
	if (bUpdateFinished)
	{
		m_ullFinished += ullSize;
	}

	return ullSize;
}

unsigned long long BDataInfo::Write(void* pBuff,
	unsigned long long cb,
	bool bUpdateFinished,
	bool bNewOrigin,
	unsigned long long ullNewPos)
{
	unsigned long long ullPos = bNewOrigin ? ullNewPos : m_ullOrigin + m_ullFinished;
	unsigned long long ullSize = min<unsigned long long>(cb, m_cb - m_ullFinished);
	memcpy((char*)m_lpv + ullPos, pBuff, ullSize);
	if (bUpdateFinished)
	{
		m_ullFinished += ullSize;
	}

	return ullSize;
}

bool BDataInfo::MapFile()
{
	m_hFileMapping = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	CHK_false(m_hFileMapping);

	m_lpv = MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE, 0, 0, 0);
	if (!m_lpv)
	{
		SAFE_CLOSE_HANDLE(m_hFileMapping);
		return false;
	}

	return true;
}

void BDataInfo::CreateContinuation(BDataContinuation& continuation)
{
	continuation.m_nID = m_nID;
	continuation.m_bDir = m_bDir;
	continuation.m_strFile = m_strFile;
	continuation.m_cb = m_cb;
	continuation.m_ullFinished = m_ullFinished;
	continuation.m_ullOrigin = m_ullOrigin;
	continuation.m_primaryPeer = m_primaryPeer;
}
