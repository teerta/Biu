
#include "stdafx.h"
#include "biodisp.h"
#include "priv/i_iodispab.h"
#include "priv/bioctxt.h"
#include "biohand.h"
#include "biorule.h"

using namespace Biu;


// BIODispatchable

BIODispatchable::BIODispatchable()
	: m_pIntern(NULL)
{
}

BIODispatchable::~BIODispatchable()
{
}

void BIODispatchable::SetTimeout(long long llSeconds)
{
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	time(&pIntern->m_tBeginning);
	pIntern->m_llTimeout = llSeconds;
}

BIOExecutionUnit* BIODispatchable::GetExecutionUnit() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_pExecUnit; 
}

void BIODispatchable::SetExecutionUnit(BIOExecutionUnit* pUnit) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_pExecUnit = pUnit; 
}

IBDataChannel* BIODispatchable::GetProtocol() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_pProtocol;
}

void BIODispatchable::SetProtocol(IBDataChannel* pProtocol) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_pProtocol = pProtocol;
}

BIOHandler* BIODispatchable::GetHandler() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_pHandler;
}

void BIODispatchable::SetHandler(BIOHandler* pHandler) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_pHandler = pHandler;
}

bool BIODispatchable::IsInTheCircle() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_bInTheCircle;
}

void BIODispatchable::SetInTheCircle(bool val) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_bInTheCircle = val;
}

#ifndef _WINDOWS
bool BIODispatchable::IsInTheCircleForWriting() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_bInTheCircleWrite; 
}

void BIODispatchable::SetInTheCircleForWriting(bool val) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_bInTheCircleWrite = val; 
}
#endif

bool BIODispatchable::IsWriting() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_bWriting;
}

void BIODispatchable::SetWriting(bool val) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_bWriting = val;
}

unsigned int BIODispatchable::GetRefIO() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_refIO;
}

void BIODispatchable::IncRefIO() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_refIO++;
}

void BIODispatchable::DecRefIO() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_refIO--;
}

long long BIODispatchable::GetVar() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_llVar;
}

void BIODispatchable::SetVar(long long llVar) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_llVar = llVar;
}

B_IO_Read_Mode BIODispatchable::GetMode() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_mode;
}

void BIODispatchable::SetMode(B_IO_Read_Mode mode) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_mode = mode;
}

long long BIODispatchable::GetTimeout() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_llTimeout;
}

void BIODispatchable::SetPending(bool b) 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->m_bPending = b;
}

bool BIODispatchable::IsPending() 
{ 
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_bPending;
}

void BIODispatchable::Clear()
{
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->Clear();
}

void BIODispatchable::ConfigSecurity(BSecurityProvider* pProv)
{
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->ConfigSecurity(pProv);
}

BIOSecurityContext* BIODispatchable::GetSecurityContext()
{
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->m_pSecurityContext;
}

void BIODispatchable::ConfigBehaviorArbiter()
{
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	pIntern->ConfigBehaviorArbiter();
}

BIOBehaviorArbiter* BIODispatchable::GetBehaviorArbiter()
{
	I_IODispatchable* pIntern = reinterpret_cast<I_IODispatchable*>(m_pIntern);

	return pIntern->GetBehaviorArbiter();
}
