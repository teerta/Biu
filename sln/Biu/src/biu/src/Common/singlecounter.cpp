
#include "stdafx.h"
#include "bcounter.h"
#include "bstdx.h"


using namespace Biu;


BSingleCounter::BSingleCounter()
	: m_nVal(0ull)
{

}

BSingleCounter::BSingleCounter(unsigned long long ullVal, unsigned long long ullMax)
	: m_nVal(ullVal)
	, m_nMax(ullMax)
{

}

BSingleCounter::BSingleCounter(const BSingleCounter& rhs)
{
	Copy(rhs);
}

BSingleCounter::~BSingleCounter()
{

}

BSingleCounter& BSingleCounter::operator = (const BSingleCounter& rhs)
{
	Copy(rhs);

	return *this;
}

void BSingleCounter::Copy(const BSingleCounter& rhs)
{
	if (&rhs == this)
	{
		return;
	}

	SAtomicAssign(m_nVal, rhs.m_nVal);
	SAtomicAssign(m_nMax, rhs.m_nMax);
}

bool BSingleCounter::CheckFailed(unsigned long long ullVal, bool bReset)
{
	bool bRet = false;
	atomic_fetch_add_explicit(&m_nVal, ullVal, memory_order_relaxed);
	unsigned long long ullCur = atomic_load_explicit(&m_nVal, memory_order_relaxed);
	unsigned long long ullMax = atomic_load_explicit(&m_nMax, memory_order_relaxed);
	if (ullCur >= ullMax)
	{
		bRet = true;
		if (bReset)
		{
			Reset();
		}
	}

	return bRet;
}
