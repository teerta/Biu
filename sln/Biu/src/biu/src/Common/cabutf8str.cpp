
#include "stdafx.h"

#if defined(_WINDOWS)

#include "biu.h"
#include "bstrenc.h"

#endif

#include "btype.h"
#include "bmarco.h"
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <locale>
#include <algorithm>


using namespace Biu;


// BCabUTF8String

BCabUTF8String::BCabUTF8String()
	: m_cb(0)
	, m_sz(NULL)
{
}

BCabUTF8String::~BCabUTF8String()
{
	SAFE_DELETE_ARRAY(m_sz);
	m_cb = 0;
}

BCabUTF8String::BCabUTF8String(const BCabUTF8String& str)
	: m_cb(0)
	, m_sz(NULL)
{
	Copy(str);
}

BCabUTF8String::BCabUTF8String(BCabUTF8String&& str)
		: m_sz(NULL)
		, m_cb(0)
{
	*this = std::move(str);
}

BCabUTF8String::BCabUTF8String(const string& str)
		: m_sz(NULL)
		, m_cb(0)
{
	m_sz = new char[str.length() + 1];
#ifdef _WINDOWS
	strcpy_s(m_sz, str.length() + 1, str.c_str());
#else
	strcpy(m_sz, str.c_str());
#endif
	m_sz[str.length()] = '\0';
	m_cb = strlen(m_sz);
}

BCabUTF8String::BCabUTF8String(const wchar_t* lpwsz)
{
#ifdef _WINDOWS
	if (lpwsz)
	{
		SAFE_DELETE_ARRAY(m_sz);
		m_cb = BEncoder::ConvertToUTF8(lpwsz, &m_sz);
	}
#else
	if (lpwsz)
	{
		SAFE_DELETE_ARRAY(m_sz);

		setlocale(LC_ALL, "Chinese");

		m_cb = wcstombs(NULL, lpwsz, 0);
		if (!m_cb)
		{
			m_cb = 0;
			return;
		}

		m_sz = new char[m_cb + 1];
		wcstombs(m_sz, lpwsz, m_cb + 1);

		m_cb = strlen(m_sz);
	}
#endif // _WINDOWS
}

//#ifdef _WINDOWS
//BCabUTF8String::BCabUTF8String(const CStringW& str)
//{
//	SAFE_DELETE_ARRAY(m_sz);
//	m_cb = BEncoder::ConvertToUTF8(str, &m_sz);
//}
//#endif

BCabUTF8String& BCabUTF8String::operator=(const BCabUTF8String& str)
{
	Copy(str);

	return *this;
}

BCabUTF8String& BCabUTF8String::operator=(BCabUTF8String&& str)
{
	if (&str != this)
	{
		SAFE_DELETE_ARRAY(m_sz);
		m_sz = str.m_sz;
		m_cb = str.m_cb;

		str.m_sz = NULL;
		str.m_cb = 0;
	}
	
	return *this;
}

BCabUTF8String& BCabUTF8String::operator=(const wchar_t* lpwsz)
{
#ifdef _WINDOWS
	if (lpwsz)
	{
		SAFE_DELETE_ARRAY(m_sz);
		m_cb = BEncoder::ConvertToUTF8(lpwsz, &m_sz);
	}
#else
	if (lpwsz)
	{
		SAFE_DELETE_ARRAY(m_sz);

		setlocale(LC_ALL, "Chinese");

		m_cb = wcstombs(NULL, lpwsz, 0);
		if (!m_cb)
		{
			m_cb = 0;
			return *this;
		}

		m_sz = new char[m_cb + 1];
		wcstombs(m_sz, lpwsz, m_cb + 1);

		m_cb = strlen(m_sz);
	}
#endif // _WINDOWS

	return *this;
}

BCabUTF8String& BCabUTF8String::operator=(const string& str)
{
	SAFE_DELETE_ARRAY(m_sz);
	m_sz = new char[str.length() + 1];
#ifdef _WINDOWS
	strcpy_s(m_sz, str.length() + 1, str.c_str());
#else
	strcpy(m_sz, str.c_str());
#endif
	m_sz[str.length()] = '\0';
	m_cb = strlen(m_sz);
	
	return *this;
}

//#ifdef _WINDOWS
//BCabUTF8String& BCabUTF8String::operator=(const CStringW& str)
//{
//	SAFE_DELETE_ARRAY(m_sz);
//	m_cb = BEncoder::ConvertToUTF8(str, &m_sz);
//
//	return *this;
//}
//#endif // _WINDOWS

void BCabUTF8String::Copy(const BCabUTF8String& str)
{
	if (&str == this)
	{
		return;
	}

	COPY_STRING_LEN(str.m_sz, m_sz, m_cb);
}

#ifdef _WINDOWS
int BCabUTF8String::ToUnicode(LPWSTR* ppszWide)
{
	if (!m_sz || !ppszWide)
	{
		return -1;
	}

	int cb = MultiByteToWideChar(CP_UTF8, NULL, m_sz, m_cb, NULL, 0);
	if (!cb)
	{
		return -1;
	}

	*ppszWide = new wchar_t[cb + 1];

	int nRet = MultiByteToWideChar(CP_UTF8, NULL, m_sz, m_cb, *ppszWide, cb + 1);
	(*ppszWide)[cb] = L'\0';

	return nRet;
}
#endif // _WINDOWS

#ifdef _WINDOWS
int BCabUTF8String::ToUnicode(CAtlStringW& str)
{
	if (!m_sz)
	{
		return -1;
	}

	int cb = MultiByteToWideChar(CP_UTF8, NULL, m_sz, m_cb, NULL, 0);
	if (!cb)
	{
		return -1;
	}

	wchar_t* pszWide = str.GetBuffer(cb + 1);

	cb = MultiByteToWideChar(CP_UTF8, NULL, m_sz, m_cb, pszWide, cb + 1);
	pszWide[cb] = L'\0';
	str.ReleaseBuffer();

	return cb;
}
#endif // _WINDOWS

BCabUTF8String::operator char*()
{
	return m_sz;
}

BCabUTF8String::operator const char*()
{
	return m_sz;
}

void BCabUTF8String::SetString(const char* psz)
{
	SAFE_DELETE_ARRAY(m_sz);
	m_cb = strlen(psz);
	m_sz = new char[m_cb + 1];
#ifdef _WINDOWS
	strcpy_s(m_sz, m_cb + 1, psz);
#else
	strcpy(m_sz, psz);
#endif
	m_sz[m_cb] = '\0';
}

void BCabUTF8String::GetString(char* psz, size_t cb)
{
	memset(psz, 0, cb);
	memcpy(psz, m_sz, min<size_t>(cb, m_cb));
//    strncpy(psz, m_sz, min<size_t>(cb, m_cb));
}
