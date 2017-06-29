
#include "stdafx.h"
#include "bencoder.h"
#include "bmarco.h"
#include "bfunc.h"
#include <cassert>
#include <cstring>
#include <limits.h>


using namespace Biu;


// BAlphabetEncoder

static const char AlphabetEncoder_Default_Alphabet[] =
{
	"[qazxswedcvfrtgbnhyujmkiolp1092387456LOPIKMJUYHNBGTRFVCDEWSXZAQ]"
};

static const int AlphabetEncoder_Min_Alpha = 16;
static const int AlphabetEncoder_Max_Alpha = 256;


BAlphabetEncoder::BAlphabetEncoder()
		: m_lpszAlphabet(NULL)
		, m_lpbPageIndicators(NULL)
		, m_lpbReversedPageIndicators(NULL)
		, m_pszAlphabetMap(NULL)
		, m_pszPageIndicatorMap(NULL)
		, m_pszReversedPageIndicatorMap(NULL)
		, m_cbAlphabet(0)
		, m_dwNumberOfPage(0)
{
}

BAlphabetEncoder::~BAlphabetEncoder()
{
	Clear();
}

void BAlphabetEncoder::Clear()
{
	SAFE_DELETE_ARRAY(m_lpszAlphabet);
	SAFE_DELETE_ARRAY(m_lpbPageIndicators);
	SAFE_DELETE_ARRAY(m_lpbReversedPageIndicators);
	SAFE_DELETE_ARRAY(m_pszAlphabetMap);
	SAFE_DELETE_ARRAY(m_pszPageIndicatorMap);
	SAFE_DELETE_ARRAY(m_pszReversedPageIndicatorMap);    
}

bool BAlphabetEncoder::SetAlphabet(const char* lpszAlphabet)
{
	assert(lpszAlphabet);

	size_t cb = strlen(lpszAlphabet);	
	assert(!(cb < AlphabetEncoder_Min_Alpha || cb > AlphabetEncoder_Max_Alpha));

	char szAlpha[AlphabetEncoder_Max_Alpha];
	memset(szAlpha, 0, AlphabetEncoder_Max_Alpha);
	for (size_t i = 0; i < cb; i++)
	{
		if (szAlpha[lpszAlphabet[i]])
		{
			return false;
		}

		szAlpha[lpszAlphabet[i]]++;
	}

	Clear();

	m_cbAlphabet = cb;
	m_lpszAlphabet = new char[m_cbAlphabet + 1];
#ifdef _WINDOWS
	strcpy_s(m_lpszAlphabet, m_cbAlphabet + 1, lpszAlphabet);
#else
	strcpy(m_lpszAlphabet, lpszAlphabet);
#endif

	m_dwNumberOfPage = AlphabetEncoder_Max_Alpha / m_cbAlphabet;
	m_lpbPageIndicators = new unsigned char[m_dwNumberOfPage];
	memset(m_lpbPageIndicators, 0, m_dwNumberOfPage);
	m_lpbPageIndicators[0] = m_lpszAlphabet[0];
	for (size_t i = 1; i < m_dwNumberOfPage; i++)
	{
		m_lpbPageIndicators[i] = m_lpszAlphabet[m_cbAlphabet / m_dwNumberOfPage * i];
	}

	m_lpbReversedPageIndicators = new unsigned char[m_dwNumberOfPage];
	memset(m_lpbReversedPageIndicators, 0, m_dwNumberOfPage);
	for (size_t i = 0; i < m_dwNumberOfPage; i++)
	{
			m_lpbReversedPageIndicators[i] = m_lpbPageIndicators[m_dwNumberOfPage - i - 1];
	}

	m_pszAlphabetMap = new char[AlphabetEncoder_Max_Alpha];
	memset(m_pszAlphabetMap, 0, AlphabetEncoder_Max_Alpha);
	for (size_t i = 0; i < m_cbAlphabet; i++)
	{
			m_pszAlphabetMap[m_lpszAlphabet[i]] = i;
	}

	m_pszPageIndicatorMap = new char[AlphabetEncoder_Max_Alpha];
	memset(m_pszPageIndicatorMap, 0, AlphabetEncoder_Max_Alpha);
	for (size_t i = 0; i < m_dwNumberOfPage; i++)
	{
			m_pszPageIndicatorMap[m_lpbPageIndicators[i]] = i;
	}

	m_pszReversedPageIndicatorMap = new char[AlphabetEncoder_Max_Alpha];
	memset(m_pszReversedPageIndicatorMap, 0, AlphabetEncoder_Max_Alpha);
	for (size_t i = 0; i < m_dwNumberOfPage; i++)
	{
			m_pszReversedPageIndicatorMap[m_lpbReversedPageIndicators[i]] = i;
	}

	return true;
}

bool BAlphabetEncoder::SetDefaultAlphabet()
{
	return SetAlphabet(AlphabetEncoder_Default_Alphabet);
}

size_t BAlphabetEncoder::GetEncodedDataSize(const char* lpszData)
{
	return strlen(lpszData) * 2 + 3;
}

size_t BAlphabetEncoder::GetDecodedDataSize(const char* lpszData)
{
	return (strlen(lpszData) - 2) / 2 + 1;
}

bool BAlphabetEncoder::Encode(const char* lpszData, char* lpszResult, size_t cb)
{
	assert(m_lpszAlphabet);
	assert(lpszData);
	assert(lpszResult);

	unsigned int nVal = BRand(0, INT_MAX);
	
	bool	bClockwise = nVal % 2;
	bool	s = true;
	size_t	d = 0,
			r = 0,
			cbResult = GetEncodedDataSize(lpszData),
			cbData = strlen(lpszData);
	if (cb)
	{
		cbData = cb;
		cbResult = cbData * 2 + 3;
	}

	int	nStart = BRand(0, m_dwNumberOfPage - 1);

	memset(lpszResult, 0, cbResult);
	lpszResult[0] = m_lpbPageIndicators[nStart];

	if (bClockwise)
	{
		// Clockwise
		lpszResult[1] = m_lpszAlphabet[2 * BRand(0, m_cbAlphabet / 2 - 1)];
	}
	else
	{
		// Anticlockwise
		int nRnd = BRand(1, m_cbAlphabet - 1);
		if (!(nRnd % 2))
		{
			nRnd--;
		}

		lpszResult[1] = m_lpszAlphabet[nRnd];
	}

	for (size_t i = 0; i < cbData; i++)
	{
		d = (unsigned char)lpszData[i] / m_cbAlphabet;
		r = (unsigned char)lpszData[i] % m_cbAlphabet;

//        if (r)
//        {
//            r--;
//        }
//        else
//        {
//            if (d)
//            {
//                d--;
//                r = m_cbAlphabet - 1;
//            }
//        }

		if (s)
		{
			lpszResult[2 + i * 2] = GetPageIndicator(bClockwise, nStart, d);
			lpszResult[2 + i * 2 + 1] = m_lpszAlphabet[r];
		}
		else
		{
			lpszResult[2 + i * 2] = m_lpszAlphabet[r];
			lpszResult[2 + i * 2 + 1] = GetPageIndicator(bClockwise, nStart, d);
		}

		s = !s;
	}

	return true;
}

bool BAlphabetEncoder::Decode(const char* lpszData, char* lpszResult)
{
	assert(m_lpszAlphabet);
	assert(lpszData);
	assert(lpszResult);

	size_t	d = 0,
			r = 0,
			cbResult = GetDecodedDataSize(lpszData),
			cbData = strlen(lpszData);

	bool	s = true;
	int	nStart = m_pszPageIndicatorMap[lpszData[0]];
	bool	bClockwise = !(m_pszAlphabetMap[lpszData[1]] % 2);

	memset(lpszResult, 0, cbResult);
	for (size_t i = 0; i < cbResult - 1; i++)
	{
		if (s)
		{
			d = GetPageIndicatorPos(bClockwise, nStart, lpszData[2 + i * 2]);
			r = m_pszAlphabetMap[lpszData[2 + i * 2 + 1]];
		}
		else
		{
			r = m_pszAlphabetMap[lpszData[2 + i * 2]];
			d = GetPageIndicatorPos(bClockwise, nStart, lpszData[2 + i * 2 + 1]);
		}

//        if (r || d)
//        {
//            if (m_cbAlphabet - 1 == r)
//            {
//                d++;
//                r = 0;
//            }
//            else
//            {
//                r++;
//            }
//        }

		lpszResult[i] = d * m_cbAlphabet + r;

		s = !s;
	}

	return true;
}

unsigned char BAlphabetEncoder::GetPageIndicator(bool bClockwise, int nStart, int nPos)
{
	return bClockwise ? 
			m_lpbPageIndicators[(nStart + nPos) % m_dwNumberOfPage] :
			m_lpbReversedPageIndicators[(m_dwNumberOfPage - nStart - 1 + nPos) % m_dwNumberOfPage];
}

unsigned char BAlphabetEncoder::GetPageIndicatorPos(bool bClockWise, int nStart, unsigned char data)
{
	return bClockWise ?
			m_pszPageIndicatorMap[data] - nStart :
			m_pszReversedPageIndicatorMap[data] - (m_dwNumberOfPage - nStart - 1);
}

bool BAlphabetEncoder::EncodeString(const char* lpszData, string& str)
{
	assert(lpszData);

	size_t cb = GetEncodedDataSize(lpszData);
	char* buff = new char[cb];
	bool bRet = Encode(lpszData, buff);

	str = buff;
	SAFE_DELETE_ARRAY(buff);

	return bRet;
}

bool BAlphabetEncoder::DecodeString(const char* lpszData, string& str)
{
	assert(lpszData);

	size_t cb = GetDecodedDataSize(lpszData);
	char* buff = new char[cb];
	bool bRet = Decode(lpszData, buff);
	
	str = buff;
	SAFE_DELETE_ARRAY(buff);

	return bRet;
}

bool BAlphabetEncoder::EncodeString(CAtlString& data, CAtlStringW& str)
{
	CAtlStringA strA = CT2A(data);
	const char* pszData = (const char*)strA;
	size_t cb = GetEncodedDataSize(strA);
	char* buff = new char[cb];
	bool bRet = Encode(pszData, buff);

	strA = buff;
	str = CA2W(strA);
	SAFE_DELETE_ARRAY(buff);

	return bRet;
}

bool BAlphabetEncoder::DecodeString(CAtlString& data, CAtlStringW& str)
{
	CAtlStringA strA = CT2A(data);
	const char* pszData = (const char*)strA;
	size_t cb = GetDecodedDataSize(pszData);
	char* buff = new char[cb];
	bool bRet = Decode(pszData, buff);

	strA = buff;
	str = CA2W(strA);
	SAFE_DELETE_ARRAY(buff);

	return bRet;
}

bool BAlphabetEncoder::EncodeStringData(const char* lpszData, size_t cbData, string& str)
{
	assert(lpszData);
	
	size_t cb = cbData * 2 + 3;
	char* buff = new char[cb];
	memset(buff, 0, cb);
	bool bRet = Encode(lpszData, buff, cbData);

	str = buff;
	SAFE_DELETE_ARRAY(buff);

	return bRet;
}

bool BAlphabetEncoder::DecodeStringData(const char* lpszData, char** ppData, size_t& cbData)
{
	assert(lpszData);
	assert(ppData);
	
	cbData = GetDecodedDataSize(lpszData);
	char* buff = new char[cbData];
	memset(buff, 0, cbData);
	if (!Decode(lpszData, buff))
	{
		delete[] buff;
		return false;
	}
	
	cbData--;
	
	*ppData = buff;
	return true;
}

bool BAlphabetEncoder::EncodeData(const unsigned char* lpbData, unsigned char** lplpResult, size_t* lpcbResult)
{
	assert(lpbData);
	assert(lplpResult);

	size_t cbResult = GetEncodedDataSize((const char*)lpbData);
	char* lpsz = new char[cbResult];
	bool bRet = Encode((const char*)lpbData, lpsz);
	if (!bRet)
	{
		delete[] lpsz;
		return bRet;
	}

	*lplpResult = (unsigned char*)lpsz;
	if (lpcbResult)
	{
		*lpcbResult = cbResult;
	}

	return true;
}

bool BAlphabetEncoder::DecodeData(const unsigned char* lpbData, unsigned char** lplpResult, size_t* lpcbResult)
{
	assert(lpbData);
	assert(lplpResult);

	size_t cbResult = GetDecodedDataSize((const char*)lpbData);
	char* lpsz = new char[cbResult];
	bool bRet = Decode((const char*)lpbData, lpsz);
	if (!bRet)
	{
		delete[] lpsz;
		return bRet;
	}

	*lplpResult = (unsigned char*)lpsz;
	if (lpcbResult)
	{
		*lpcbResult = cbResult;
	}

	return true;
}
