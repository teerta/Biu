
#include "stdafx.h"
#include "bcounter.h"
#include "bstdx.h"


using namespace Biu;


BMultipleCounter::BMultipleCounter()
	: m_nVal(0ull)
{
	m_bEnabled = ATOMIC_VAR_INIT(true);
}

BMultipleCounter::BMultipleCounter(unsigned long long ullVal, unsigned long long ullBad,
	unsigned long long ullWarning, unsigned long long ullBan, unsigned long long ullBannedTimeout)
	: m_nVal(ullVal)
	, m_nBad(ullBad)
	, m_nWarning(ullWarning)
	, m_nBan(ullBan)
	, m_nBannedTimeout(ullBannedTimeout)
{
	m_bEnabled = ATOMIC_VAR_INIT(true);
}

BMultipleCounter::BMultipleCounter(const BMultipleCounter& rhs)
{
	Copy(rhs);
}

BMultipleCounter::~BMultipleCounter()
{

}

BMultipleCounter& BMultipleCounter::operator = (const BMultipleCounter& rhs)
{
	Copy(rhs);
	return *this;
}

BMultipleCounter::CheckResult BMultipleCounter::Check(unsigned long long ullVal, bool bReset)
{
	if (ullVal <= 0)
	{
		return CheckResult::OK;
	}

	CheckResult result = CheckResult::OK;
	unsigned long long ullCur = m_nVal += ullVal;
	unsigned long long ullTotal = m_nTotal += ullVal;
	unsigned long long ullBad = m_nBad;
	unsigned long long ullWarning = m_nWarning;
	unsigned long long ullBan = m_nBan;
	if (ullTotal >= ullWarning)
	{
		result = CheckResult::Warning;
	}
	else if (ullTotal >= ullBan)
	{
		result = CheckResult::Banned;
	}
	else
	{
		if (ullCur >= ullBad)
		{
			result = CheckResult::Failed;
			if (bReset)
			{
				Reset();
			}
		}
	}

	return result;
}

void BMultipleCounter::Copy(const BMultipleCounter& rhs)
{
	SAtomicAssign(m_nVal, rhs.m_nVal);
	SAtomicAssign(m_nTotal, rhs.m_nTotal);
	SAtomicAssign(m_nBad, rhs.m_nBad);
	SAtomicAssign(m_nWarning, rhs.m_nWarning);
	SAtomicAssign(m_nBan, rhs.m_nBan);
	SAtomicAssign(m_bLog, rhs.m_bLog);
	SAtomicAssign(m_bEnabled, rhs.m_bEnabled);
	SAtomicAssign(m_nBannedTimeout, rhs.m_nBannedTimeout);
}
