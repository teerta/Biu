
#include "stdafx.h"
#include "bmshack.h"
#include <algorithm>


using namespace Biu;
using namespace std;


// BMemoryBrick

BMemoryBrick::BMemoryBrick(char* pLoc, const size_t cbBrick)
		: m_pLoc(pLoc)
		, m_cbBrick(cbBrick)
		, m_nFreeSize(m_cbBrick)
		, m_bIsEndOfSeries(false)
		, m_pCounterpart(NULL)
		, m_pNext(NULL)
{
}

	BMemoryBrick::BMemoryBrick(const BMemoryBrick& rhs)
	{
		Copy(rhs);
	}

	BMemoryBrick& BMemoryBrick::operator=(const BMemoryBrick& rhs)
	{
		if (this != &rhs)
		{
			Copy(rhs);
		}
		
		return *this;
	}
	
	void BMemoryBrick::Copy(const BMemoryBrick& rhs)
	{
//        m_pLoc = rhs.m_pLoc;
		m_cbBrick = rhs.m_cbBrick;
		m_nFreeSize = rhs.m_nFreeSize;
		m_bIsEndOfSeries = rhs.m_bIsEndOfSeries;
		m_pCounterpart = rhs.m_pCounterpart;
		m_pNext = rhs.m_pNext;
		
		memcpy(m_pLoc, rhs.m_pLoc, m_cbBrick);
	}


// Functions

size_t Biu::BFindBrickPositionByData(BMemoryBrick* pBrick, size_t nDataPos)
{
	size_t nRet = 1;
	while (pBrick && nDataPos > pBrick->GetDataSize())
	{
		nDataPos -= pBrick->GetDataSize();
		pBrick = pBrick->GetNext();
		nRet++;
	}
	
	return nRet;
}

BMemoryBrick* Biu::BFindBrickXY(BMemoryBrick* pBrick, size_t& nDataPos)
{
	auto* p = pBrick;
	size_t nPos = nDataPos;
	while (p && nPos > p->GetDataSize())
	{
		nPos -= p->GetDataSize();
		p = p->GetNext();
	}

	
	if (p)
	{
		pBrick = p;
		nDataPos = nPos;
	}

	return pBrick;
}

BMemoryBrick* Biu::BFindBrickPrevious(BMemoryBrick* pBrick, BMemoryBrick* pHead)
{
	if (!pBrick || pHead == pBrick)
	{
		return NULL;
	}

	BMemoryBrick* p = NULL;
	while (pHead)
	{
		p = pHead->GetNext();
		if (p == pBrick)
		{
			return p;
		}

		pHead = p;
	}

	return NULL;
}
