
#include "stdafx.h"
#include "biorule.h"
#include "bcounter.h"
#include "bmarco.h"


using namespace Biu;


// BIOBehaviorArbiter

BIOBehaviorArbiter::BIOBehaviorArbiter()
	: m_pGlbHalfOpenConnection(__nullptr)
	, m_pDcLessData(__nullptr)
	, m_pDcFailed(__nullptr)
	, m_pDcInvalidSecurity(__nullptr)
	, m_pDcDecryptFailed(__nullptr)
	, m_pDcHashFailed(__nullptr)
	, m_pDcSigFailed(__nullptr)
	, m_pHanFailed(__nullptr)
{
	m_pGlbHalfOpenConnection = new BMultipleCounter(0, 1, 2, 3, 1200);
	m_pDcLessData = new BMultipleCounter(0, 20, 60, 100, 600);
	m_pDcFailed = new BMultipleCounter(0, 20, 60, 100, 900);
	m_pDcInvalidSecurity = new BMultipleCounter(0, 10, 20, 30, 300);
	m_pDcDecryptFailed = new BMultipleCounter(0, 15, 45, 60, 600);
	m_pDcHashFailed = new BMultipleCounter(0, 15, 45, 60, 600);
	m_pDcSigFailed = new BMultipleCounter(0, 15, 45, 60, 600);
	m_pHanFailed = new BMultipleCounter(0, 50, 100, 150, 900);

	m_pDcLessData->SetEnabled(false);
}

BIOBehaviorArbiter::~BIOBehaviorArbiter()
{
	SAFE_DELETE(m_pGlbHalfOpenConnection);
	SAFE_DELETE(m_pDcLessData);
	SAFE_DELETE(m_pDcFailed);
	SAFE_DELETE(m_pDcInvalidSecurity);
	SAFE_DELETE(m_pDcDecryptFailed);
	SAFE_DELETE(m_pDcHashFailed);
	SAFE_DELETE(m_pDcSigFailed);
	SAFE_DELETE(m_pDcHashFailed);
}

BIOBehaviorArbiter::BIOBehaviorArbiter(const BIOBehaviorArbiter& rhs)
{
	Copy(rhs);
}

BIOBehaviorArbiter& BIOBehaviorArbiter::operator=(const BIOBehaviorArbiter& rhs)
{
	Copy(rhs);
	return *this;
}

void BIOBehaviorArbiter::Copy(const BIOBehaviorArbiter& rhs)
{
	if (&rhs == this)
	{
		return;
	}

	*m_pGlbHalfOpenConnection = *rhs.m_pGlbHalfOpenConnection;
	*m_pDcLessData = *rhs.m_pDcLessData;
	*m_pDcFailed = *rhs.m_pDcFailed;
	*m_pDcInvalidSecurity = *rhs.m_pDcInvalidSecurity;
	*m_pDcDecryptFailed = *rhs.m_pDcDecryptFailed;
	*m_pDcHashFailed = *rhs.m_pDcHashFailed;
	*m_pDcSigFailed = *rhs.m_pDcSigFailed;
	*m_pHanFailed = *rhs.m_pHanFailed;
}
