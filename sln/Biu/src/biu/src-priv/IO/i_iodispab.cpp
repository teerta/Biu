
#include "stdafx.h"
#include "priv/i_iodispab.h"
#include "priv/bioctxt.h"
#include "biosecc.h"
#include "biorule.h"


using namespace Biu;


// I_IODispatchable

I_IODispatchable::I_IODispatchable()
	: m_pOutter(NULL)
	, m_pExecUnit(NULL)
	, m_pProtocol(NULL)
	, m_pHandler(NULL)
	, m_pReadContext(NULL)
	, m_pWriteContext(NULL)
	, m_tBeginning(0)
	, m_llTimeout(-1)
	, m_llVar(0)
	, m_mode(BIO_RM_Serial)
	, m_pSecurityContext(NULL)
	, m_pBehaviorArbiter(NULL)
{
	m_bReading = false;
	m_bWriting = false;
	m_bInTheCircle = false;
#ifndef _WINDOWS
	m_bInTheCircleWrite = false;
#endif
	m_refIO = 0;
	m_refReadIO = 0;
	m_bPending = false;

	m_pReadContext = new BIOContext();
	m_pWriteContext = new BIOContext();
}

I_IODispatchable::~I_IODispatchable()
{
	SAFE_DELETE(m_pProtocol);
	SAFE_DELETE(m_pReadContext);
	SAFE_DELETE(m_pWriteContext);
	SAFE_DELETE(m_pSecurityContext);
	SAFE_DELETE(m_pBehaviorArbiter);
}

void I_IODispatchable::Clear()
{
	//m_pExecUnit = NULL;
	//m_pProtocol = NULL;
	SAFE_DELETE(m_pProtocol);

	m_pReadContext->Clear();
	m_pReadContext->m_operation = BIO_Accept;

	m_pWriteContext->Clear();
	m_pWriteContext->m_operation = BIO_Write;

	m_bReading = false;
	m_bWriting = false;
	m_bInTheCircle = false;
#ifndef _WINDOWS
	m_bInTheCircleWrite = false;
#endif
	m_refIO = 0;
	m_refReadIO = 0;
	m_tBeginning = 0;
	m_llTimeout = -1;
	m_llVar = 0;
	m_mode = BIO_RM_Serial;

	SAFE_DELETE(m_pSecurityContext);
	SAFE_DELETE(m_pBehaviorArbiter);
}

void I_IODispatchable::ConfigSecurity(BSecurityProvider* pProv)
{
	if (m_pSecurityContext)
	{
		return;
	}

	m_pSecurityContext = new BIOSecurityContext(pProv);
}

void I_IODispatchable::ConfigBehaviorArbiter()
{
	if (m_pBehaviorArbiter)
	{
		return;
	}

	m_pBehaviorArbiter = new BIOBehaviorArbiter();
}

BIOBehaviorArbiter* I_IODispatchable::GetBehaviorArbiter()
{
	return m_pBehaviorArbiter;
}
