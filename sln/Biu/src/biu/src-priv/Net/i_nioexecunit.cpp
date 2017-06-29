
#include "stdafx.h"
#include "priv/i_nio.h"
#include "priv/i_peer.h"
#include "bbfunc.h"
#include "bnio.h"
#include "priv/bioctxt.h"
#include "biosecc.h"


// I_NetIOExecutionUnit

I_NetIOExecutionUnit::I_NetIOExecutionUnit()
	: m_pOutter(NULL)
{

}

I_NetIOExecutionUnit::~I_NetIOExecutionUnit()
{
	DeleteContainer2nd(m_clientData);
	DeleteContainer2nd(m_data);

	m_clients.ClearPointer();
	m_servers.ClearPointer();

#ifndef _WINDOWS
	{
		BScopeLock sl(&m_csPendingConnect);
		DestructContainer(m_pengingConnect, [](BPeer* peer){ delete peer; });
	}
#endif

	{
		BScopeLock sl(&m_csIdleProactiveClosed);
		DestructContainer(m_listProactiveClosed, [](BPeer* peer){ delete peer; });
	}

	{
		BScopeLock sl(&m_csIdlePassiveClosed);
		DestructContainer(m_listPassiveClosed, [](BPeer* peer){ delete peer; });
	}
}


// TODO: handle pending connection (windows: pending-accept) (linux: pending-connect)
void I_NetIOExecutionUnit::HandlePending()
{
#ifdef _WINDOWS
#else
	if (!m_csPendingConnect.TryLock())
	{
		return;
	}

	time_t tNow;
	time(&tNow);
	list<BPeer*> list;
	{
		//        BScopeLock sl(&m_csPendingConnect);
		for_each(begin(m_pengingConnect), end(m_pengingConnect), [&](BPeer* peer)
		{
			if (-1 != peer->m_llTimeout)
			{
				if (tNow - peer->m_tBeginning > peer->m_llTimeout)
				{
					list.push_back(peer);
				}
			}
		});

		for_each(begin(list), end(list), [&](BPeer* peer)
		{
			m_pengingConnect.remove(peer);
		});
	}

	m_csPendingConnect.Unlock();

	for_each(begin(list), end(list), [&](BPeer* peer)
	{
		peer->NotifyTimeout();
		//peer->RemoveFromCollection();
		peer->Destroy();
	});
#endif
}

BPeer* I_NetIOExecutionUnit::AcquirePeer(B_Sock_Type type, bool accept)
{
	BPeer* peer = NULL;
	if (accept)
	{
		// Passive Closed

		BScopeLock sl(&m_csIdlePassiveClosed);
		if (!m_listPassiveClosed.empty())
		{
			auto iter = find_if(m_listPassiveClosed.begin(), m_listPassiveClosed.end(),
				[&](BPeer* p)
			{
				return type == p->GetPeerSockType();
			}
			);

			if (m_listPassiveClosed.end() != iter)
			{
				peer = *iter;
				m_listPassiveClosed.erase(iter);
			}
		}
	}

	if (!peer)
	{
		// Priactive Closed

		BScopeLock sl(&m_csIdleProactiveClosed);
		if (!m_listProactiveClosed.empty())
		{
			auto iter = find_if(m_listProactiveClosed.begin(), m_listProactiveClosed.end(),
				[&](BPeer* p)
			{
				return type == p->GetPeerSockType();
			}
			);

			if (m_listProactiveClosed.end() != iter)
			{
				peer = *iter;
				m_listProactiveClosed.erase(iter);
			}
		}
	}
	
	if (!peer)
	{
		peer = new BPeer();
	}
	
	return peer;
}

bool I_NetIOExecutionUnit::PostAccept(BPeer* peer, unsigned int dwCount)
{
	BPeer* pAccept = NULL;
	I_Peer* pIntern = NULL;
	for (UINT32 i = 0; i < dwCount; i++)
	{
		pAccept = AcquirePeer(peer->GetPeerSockType(), true);
		pIntern = reinterpret_cast<I_Peer*>(pAccept->GetIntern());
		if (!pIntern->CreateAccept(peer))
		{
			pIntern->NotifyError(BCS_NIoExec_PostAccept_CreateAccept, WSAGetLastError());
			return false;
		}

		auto* pSec = peer->GetSecurityContext();
		if (pSec)
		{
			pIntern->ConfigSecurity(pSec->GetSecurity());
		}

		//if (!m_pDispatcher->JoinTheCircle(pAccept))
		//{
		//	delete pAccept;
		//	return false;
		//}

		//pAccept->SetServerID(nCookie);
		int nErr = pIntern->BeginAccept(peer);
		if (nErr)
		{
			pIntern->NotifyError(BCS_NIoExec_PostAccept_BeginAccept, nErr);
			pIntern->Destroy();
			return false;
		}
	}

	return true;
}


//#ifdef _WINDOWS
//bool I_NetIOExecutionUnit::Listen(
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
//#else
//bool I_NetIOExecutionUnit::Listen(
//	B_Sock_Type sockType,
//	sockaddr_in* psa,
//	unsigned int& nCookie,
//	long long llTimeoutSeconds,
//	int nBacklog,
//	long long llVar,
//	bool bCompatibleInitializationState
//	)
//#endif
bool I_NetIOExecutionUnit::Listen(BNConnectionCreation& ncc, unsigned int& nCookie)
{
	assert(ncc.nBacklog);

	if (BSTR_Data == ncc.trait)
	{
		CHK_false(!BSyncGetPointer(m_csData, m_data, nCookie));
	}

	BPeer* peer = new BPeer();
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	if (!pIntern->Create(ncc.sockType, m_pOutter, ncc.pHandler, ncc.pProtocol, BIO_Accept))
	{
		delete peer;
		return false;
	}

	if (!pIntern->ActAsServer(&ncc.sa, ncc.nBacklog))
	{
		// handle error here
		delete peer;
		return false;
	}

	switch (ncc.trait)
	{
	case BSTR_Normal:
	{
		BServerPeer* pServer = new BServerPeer(peer);
		pServer->SetRunning();
		nCookie = AddServer(pServer);
	}
		break;

	case BSTR_Data:
	{
		BDataPeer* pData = new BDataPeer(peer);
		pData->SetRunning();
		BSyncAdd(m_csData, m_data, nCookie, pData);
		pIntern->m_trait = BSTR_Data;
	}
		break;

	default:
		break;
	}

	pIntern->m_bCompatibleInitialization = ncc.bCompatibleInitializationState;
	pIntern->m_nID = nCookie;
	pIntern->m_nServerID = nCookie;
	pIntern->m_llVar = ncc.llVar;
	peer->SetTimeout(ncc.llTimeoutSeconds);

	if (ncc.pSec)
	{
		pIntern->ConfigSecurity(ncc.pSec);
	}

	if (!m_pOutter->GetDispatcher()->JoinTheCircle(peer))
	{
		// handle error here
		RemoveServer(nCookie);
		return false;
	}

#ifdef _WINDOWS
	return PostAccept(peer, ncc.nAccepts);
#else
	return true;
#endif
}

//bool I_NetIOExecutionUnit::Connect(
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
bool I_NetIOExecutionUnit::Connect(BNConnectionCreation& ncc, unsigned int& nCookie)
{
	BPeer* peer = AcquirePeer(ncc.sockType, false);
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	if (!pIntern->Create(ncc.sockType, m_pOutter, ncc.pHandler, ncc.pProtocol, BIO_Connect))
	{
		delete peer;
		return false;
	}

	if (!pIntern->ActAsClient(&ncc.sa))
	{
		delete peer;
		return false;
	}

	pIntern->m_trait = ncc.trait;
	pIntern->m_bCompatibleInitialization = ncc.bCompatibleInitializationState;
	pIntern->m_nRefID = ncc.nRef;
	pIntern->m_llVar = ncc.llVar;

	nCookie = AddClient(peer);
	pIntern->m_nID = nCookie;

#ifdef _WINDOWS
	if (!m_pOutter->GetDispatcher()->JoinTheCircle(peer))
#else
	if (!m_pDispatcher->JoinTheCircleForWriting(peer))
#endif
	{
		// handle error here
		RemoveClient(nCookie);
		return false;
	}

	peer->SetTimeout(ncc.llTimeoutSeconds);
	peer->SetPending();

	if (ncc.pSec)
	{
		pIntern->ConfigSecurity(ncc.pSec);
	}

#ifdef _WINDOWS
	int nRet = pIntern->BeginConnect(&ncc.saRemote);
	if (nRet)
	{
		peer->HandleError(BCS_NIoExec_Connect_BeginConnect, nRet);
		return false;
	}
#else
	int nRet = peer->Connect(psaRemote);
	if (nRet && EINPROGRESS != nRet)
	{
		// handle other errno
		return false;
	}
#endif

	return true;
}

bool I_NetIOExecutionUnit::ConnectData(B_Sock_Trait trait, unsigned int nCookie)
{
	auto* peer = GetClient(nCookie);
	CHK_false(peer);

	bool bRet = false;
	switch (trait)
	{
	case BSTR_Data:
	{
		BClientDataPeer* pClientData = new BClientDataPeer(peer);
		BSyncAdd(m_csClientData, m_clientData, nCookie, pClientData);
		bRet = true;
	}
		break;
	}

	return bRet;
}

BPeer* I_NetIOExecutionUnit::AcquirePeerForConnecting(BNConnectionCreation& ncc, unsigned int& nCookie)
{
	BPeer* peer = AcquirePeer(ncc.sockType, false);
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	if (!pIntern->Create(ncc.sockType, m_pOutter, ncc.pHandler, ncc.pProtocol, BIO_Connect))
	{
		delete peer;
		return false;
	}

	if (!pIntern->ActAsClient(&ncc.sa))
	{
		delete peer;
		return false;
	}

	pIntern->m_trait = ncc.trait;
	pIntern->m_bCompatibleInitialization = ncc.bCompatibleInitializationState;
	pIntern->m_nRefID = ncc.nRef;
	pIntern->m_llVar = ncc.llVar;

	nCookie = AddClient(peer);
	pIntern->m_nID = nCookie;

	return peer;
}

bool I_NetIOExecutionUnit::Connect(BPeer* peer, BNConnectionCreation& ncc, unsigned int& nCookie)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());

#ifdef _WINDOWS
	if (!m_pOutter->GetDispatcher()->JoinTheCircle(peer))
#else
	if (!m_pDispatcher->JoinTheCircleForWriting(peer))
#endif
	{
		// handle error here
		RemoveClient(nCookie);
		return false;
	}

	peer->SetTimeout(ncc.llTimeoutSeconds);
	peer->SetPending();

	if (ncc.pSec)
	{
		pIntern->ConfigSecurity(ncc.pSec);
	}

#ifdef _WINDOWS
	int nRet = pIntern->BeginConnect(&ncc.saRemote);
	if (nRet)
	{
		peer->HandleError(BCS_NIoExec_Connect_BeginConnect, nRet);
		return false;
	}
#else
	int nRet = peer->Connect(psaRemote);
	if (nRet && EINPROGRESS != nRet)
	{
		// handle other errno
		return false;
	}
#endif

	return true;
}

void I_NetIOExecutionUnit::CloseServer(const unsigned int nCookie)
{
	BServerPeer* pServer = NULL;
	{
		BScopeLock sl(&m_csServers);
		m_servers.GetValue(nCookie, pServer);
		if (pServer)
		{
			m_servers.Remove(nCookie);
		}
	}

	if (pServer)
	{
		auto* pData = BSyncGetPointer2(m_csData, m_data, nCookie);
		if (pData)
		{
			pData->SetTerminate(false);
			SAFE_DELETE(pData);
		}
		
		pServer->SetTerminate(false);
		SAFE_DELETE(pServer);
	}	
}

void I_NetIOExecutionUnit::CloseServerItem(unsigned int nCookie, BPeerRepresentation* pRepresentation)
{
	CHK(pRepresentation);

	BServerPeer* pServer = NULL;
	{
		BScopeLock sl(&m_csServers);
		m_servers.GetValue(nCookie, pServer);
	}
	CHK(pServer);

	auto* peer = pServer->RemovePeer(pRepresentation);
	CHK(peer);

	auto* pData = BSyncGetPointer2(m_csData, m_data, nCookie, false);
	if (pData)
	{
		pData->TerminateTransmit(peer);
	}

	reinterpret_cast<I_Peer*>(peer->GetIntern())->Destroy();
}

void I_NetIOExecutionUnit::CloseClient(const unsigned int nCookie, bool bNow)
{
	BPeer* peer = NULL;
	{
		BScopeLock sl(&m_csClients);
		m_clients.GetValue(nCookie, peer);
		if (peer)
		{
			m_clients.Remove(nCookie);
		}
	}

	if (peer)
	{
		auto* pData = BSyncGetPointer2(m_csClientData, m_clientData, nCookie);
		SAFE_DELETE(pData);
	}

	if (peer)
	{
		I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
		pIntern->Destroy();
	}
}

void I_NetIOExecutionUnit::Reclaim(BIODispatchable* pDisp)
{
	bool bProactive = true;
	BPeer* peer = static_cast<BPeer*>(pDisp);
	if (peer->IsServer())
	{
		// Server-Listen-Peer

		RemoveServer(peer->GetID());

		auto* pData = BSyncGetPointer2(m_csData, m_data, peer->GetID());
		SAFE_DELETE(pData);
	}
	else
	{
		if (peer->GetServerID())
		{
			// Server-Accept-Peer

			auto* pData = BSyncGetPointer2(m_csData, m_data, peer->GetServerID(), false);
			if (pData)
			{
				pData->TerminateTransmit(peer);
			}
			
			bProactive = false;
			I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
			pIntern->m_pServer->RemovePeer(peer);
		}
		else
		{
			// Client-Connect-Peer

			auto* pData = BSyncGetPointer2(m_csClientData, m_clientData, peer->GetID());
			SAFE_DELETE(pData);

			BScopeLock sl(&m_csClients);
			m_clients.Remove(peer->GetID());
		}
	}

	peer->Clear();
	if (bProactive)
	{
		// Proactive close

		AddProactiveClosed(peer);
	}
	else
	{
		// Passive close

		AddPassiveClosed(peer);
	}

	//if (peer->m_pServer)
	//{
	//	peer->m_pServer->RemovePeer(peer->GetRepresentation());
	//}
	//else
	//{
	//	if (peer->m_nServerID)
	//	{
	//		RemoveServer(peer->GetID());
	//	}
	//	else
	//	{
	//		{
	//			BScopeLock sl(&m_csClients);
	//			m_clients.Remove(peer->GetID());
	//		}			

	//		peer->Clear();
	//		if (BPCF_Reuse & peer->m_closeFlag)
	//		{
	//			// Passive close

	//			AddPassiveClosed(peer);
	//		}
	//		else
	//		{
	//			// Proactive close

	//			AddProactiveClosed(peer);
	//		}
	//	}
	//}	
}
