
#include "stdafx.h"
#include "bpeersvr.h"
#include "bbfunc.h"
#include "bstdcollutil.h"
#include "priv/i_peer.h"

#include <cassert>
#include <algorithm>


using namespace std;
using namespace Biu;


// BServerPeer

BServerPeer::BServerPeer()
		: m_pSP(NULL)
		, m_mode(BIO_RM_Serial)
		, m_bTerminate(true)
#ifdef _WINDOWS
		, m_bRunning(0L)
#endif
		, m_pBehaviorArbiter(NULL)
{
#ifndef _WINDOWS
	m_bRunning = false;
#endif
	m_pBehaviorArbiter = new BIOBehaviorArbiter();
}

BServerPeer::BServerPeer(BPeer* peer)
	: m_pSP(peer)
	, m_mode(BIO_RM_Serial)
	, m_bTerminate(true)
{
	m_bRunning = false;
	auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	pIntern->SetServer(this);
	m_pBehaviorArbiter = new BIOBehaviorArbiter();
}

BServerPeer::~BServerPeer()
{
	Close();
	SAFE_DELETE(m_pBehaviorArbiter);
}

BServerPeer::BServerPeer(BServerPeer& rhs)
{
	Copy(rhs);
}

BServerPeer& BServerPeer::operator=(BServerPeer& rhs)
{
	if (this != &rhs)
	{
		Copy(rhs);
	}
	return *this;
}

BPeer* BServerPeer::operator[](BPeerRepresentation* pRepresentation)
{
	BPeer* peer = NULL;
	{
		BScopeLock sl(&m_cs);
		peer = m_peers[pRepresentation];
	}
	
	return peer;
}

BPeer* BServerPeer::GetPeer(unsigned int nID)
{
	BPeer* peer = NULL;
	{
		BScopeLock sl(&m_cs);
		auto iter = find_if(begin(m_peers), end(m_peers), [&](pair<BPeerRepresentation*, BPeer*> pa)
		{
			return nID == pa.second->GetID();
		});
		
		if (end(m_peers) != iter)
		{
			peer = iter->second;
		}
	}
	
	return peer;
}

void BServerPeer::Copy(BServerPeer& rhs)
{
	m_pSP = rhs.GetServerPeer();

	BScopeLock sl(&m_cs);
	BScopeLock slR(&rhs.m_cs);
	
	m_peers.clear();
	m_peers.insert(rhs.m_peers.cbegin(), rhs.m_peers.cend());
}

void BServerPeer::AddPeer(BPeer* peer)
{
	auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	pIntern->SetID(peer->GetRemoteRepresentation()->GetID());
	
	BScopeLock sl(&m_cs);
	
	m_peers[peer->GetRemoteRepresentation()] = peer;
}

void BServerPeer::RemovePeer(BPeer* peer)
{
	assert(peer);

	BScopeLock sl(&m_cs);
	m_peers.erase(peer->GetRemoteRepresentation());
}

BPeer* BServerPeer::RemovePeer(BPeerRepresentation* pRepresentation)
{
	assert(pRepresentation);

	BPeer* peer = NULL;
	{
		BScopeLock sl(&m_cs);
		auto iter = m_peers.find(pRepresentation);
		if (iter != m_peers.end())
		{
			peer = iter->second;
			m_peers.erase(pRepresentation);
		}		
	}
	
	return peer;
}

void BServerPeer::Close()
{
	CloseServer();
	CloseClients();

	{
		BScopeLock sl(&m_csPendingAccept);

		DestructContainerMovable2nd(m_pendingAccept,
			[&](BPeer* peer)
		{
			if (m_bTerminate)
			{
				delete peer;
			}
			else
			{
				auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
				pIntern->Destroy();
			}

		});
	}
}

void BServerPeer::CloseServer()
{
	m_bRunning = false;

	if (m_pSP)
	{
		if (m_bTerminate)
		{
			delete m_pSP;
		}
		else
		{
			auto* pIntern = reinterpret_cast<I_Peer*>(m_pSP->GetIntern());
			pIntern->Destroy();
		}
		m_pSP = NULL;
	}
}

void BServerPeer::CloseClients()
{
	BScopeLock sl(&m_cs);

	DestructContainerMovable2nd(m_peers, 
			[&](BPeer* peer)
			{
				if (m_bTerminate)
				{
					delete peer;
				}
				else
				{
					auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
					pIntern->Destroy();
				}
				
			});
}

BPeer* BServerPeer::GetPeer(BPeerRepresentation* pRepresentation)
{
	auto iter = end(m_peers);
	{
		BScopeLock sl(&m_cs);
		iter = m_peers.find(pRepresentation);
	}

	return end(m_peers) == iter ? NULL : iter->second;
}

void BServerPeer::AddPendingAccept(BPeer* peer)
{
	BSyncAdd(m_csPendingAccept, m_pendingAccept, peer->GetRepresentation(), peer);
}

void BServerPeer::RemovePendingAccept(BPeer* peer)
{
	BSyncGetPointer2(m_csPendingAccept, m_pendingAccept, peer->GetRepresentation());
}

void BServerPeer::FinishPending(BPeer* peer, bool bSucc)
{
	BSyncGetPointer2(m_csPendingAccept, m_pendingAccept, peer->GetRepresentation());
	if (bSucc)
	{
		AddPeer(peer);
	}
	else
	{
		auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
		pIntern->Destroy();		
	}
}
