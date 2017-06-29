
#include "stdafx.h"
#include "bnio.h"
#include "priv/i_nio.h"
#include "bbfunc.h"


// BNetIOExecutionUnit

BNetIOExecutionUnit::BNetIOExecutionUnit()
	: m_pIntern(NULL)
{
	auto* pIntern = new I_NetIOExecutionUnit();
	pIntern->m_pOutter = this;
	m_pIntern = (BHANDLE)pIntern;
}

BNetIOExecutionUnit::~BNetIOExecutionUnit()
{
	CHK(m_pIntern);

	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	delete pIntern;
	m_pIntern = NULL;
}

void BNetIOExecutionUnit::ExecuteIO(DWORD cb, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped, BOOL bSucc)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	pIntern->ExecuteIO(cb, lpCompletionKey, lpOverlapped, bSucc);
}

#ifdef _WINDOWS
void BNetIOExecutionUnit::ExecuteWorker(BIOContext* pContext)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	pIntern->ExecuteWorker(pContext);
}
#else
bool BNetIOExecutionUnit::ExecuteWorker(BIOContext* pContext, shared_ptr<list<BIOContext*> >& listPending)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	pIntern->ExecuteWorker(pContext, listPending);
}
#endif

//#ifdef _WINDOWS
//bool BNetIOExecutionUnit::Listen(
//	B_Sock_Trait trait,
//	B_Sock_Type sockType,
//	IBDataChannel* pProtocol,
//	BIOHandler* pHandler,
//	sockaddr_in* psa,
//	unsigned int& nCookie,
//	unsigned int nAccepts,
//	long long llTimeoutSeconds,
//	int nBacklog,
//	long long llVar,
//	bool bCompatibleInitializationState
//	)
//{
//	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
//	return pIntern->Listen(trait, sockType, pProtocol, pHandler, psa, nCookie, nAccepts, llTimeoutSeconds, nBacklog, llVar, bCompatibleInitializationState);
//}
//#else
//bool BNetIOExecutionUnit::Listen(
//	B_Sock_Type sockType,
//	sockaddr_in* psa,
//	unsigned int& nCookie,
//	long long llTimeoutSeconds,
//	int nBacklog,
//	long long llVar,
//	bool bCompatibleInitializationState
//	)
//{
//	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
//	return pIntern->Listen(sockType, psa, nCookie, llTimeoutSeconds, nBacklog, llVar, nBacklog, bCompatibleInitializationState,);
//}
//#endif
//
//bool BNetIOExecutionUnit::Connect(
//	B_Sock_Trait trait,
//	B_Sock_Type sockType,
//	IBDataChannel* pProtocol,
//	BIOHandler* pHandler,
//	sockaddr_in* psa,
//	sockaddr_in* psaRemote,
//	unsigned int& nCookie,
//	long long llTimeoutSeconds,
//	unsigned int nRef,
//	long long llVar,
//	bool bCompatibleInitializationState
//	)
//{
//	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
//	return pIntern->Connect(trait, sockType, pProtocol, pHandler, psa, psaRemote, nCookie, llTimeoutSeconds, nRef, llVar, bCompatibleInitializationState);
//}

bool BNetIOExecutionUnit::Listen(BNConnectionCreation& ncc, unsigned int& nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->Listen(ncc, nCookie);
}

bool BNetIOExecutionUnit::Connect(BNConnectionCreation& ncc, unsigned int& nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->Connect(ncc, nCookie);
}

bool BNetIOExecutionUnit::ConnectData(B_Sock_Trait trait, unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->ConnectData(trait, nCookie);
}

BPeer* BNetIOExecutionUnit::AcquirePeerForConnecting(BNConnectionCreation& ncc, unsigned int& nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->AcquirePeerForConnecting(ncc, nCookie);
}

bool BNetIOExecutionUnit::Connect(BPeer* peer, BNConnectionCreation& ncc, unsigned int& nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->Connect(peer, ncc, nCookie);
}

void BNetIOExecutionUnit::CloseServer(const unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	pIntern->CloseServer(nCookie);
}

void BNetIOExecutionUnit::CloseServerItem(unsigned int nCookie, BPeerRepresentation* pRepresentation)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	pIntern->CloseServerItem(nCookie, pRepresentation);
}

void BNetIOExecutionUnit::CloseClient(const unsigned int nCookie, bool bNow)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	pIntern->CloseClient(nCookie, bNow);
}

void BNetIOExecutionUnit::Reclaim(BIODispatchable* pDisp)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	pIntern->Reclaim(pDisp);
}

bool BNetIOExecutionUnit::ContainsClient(unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->ContainsClient(nCookie);
}

bool BNetIOExecutionUnit::ContainsServer(unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->ContainsServer(nCookie);
}

BPeer* BNetIOExecutionUnit::GetClient(unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->GetClient(nCookie);
}

BServerPeer* BNetIOExecutionUnit::GetServer(unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->GetServer(nCookie);
}

BDataPeer* BNetIOExecutionUnit::GetData(unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->GetData(nCookie);
}

BClientDataPeer* BNetIOExecutionUnit::GetClientData(unsigned int nCookie)
{
	I_NetIOExecutionUnit* pIntern = reinterpret_cast<I_NetIOExecutionUnit*>(m_pIntern);
	return pIntern->GetClientData(nCookie);
}
