
#include "stdafx.h"
#include "bpeerdata.h"
#include "bbfunc.h"
#include "priv/i_peer.h"

#include <cassert>
#include <algorithm>


using namespace std;
using namespace Biu;


// BDataPeer

BDataPeer::BDataPeer(BPeer* peer)
	: BServerPeer(peer)
{
	m_nMaxSend = -1;
	m_nMaxRecv = -1;
}

BDataPeer::~BDataPeer()
{	
	DeleteContainer2nd(m_mapPeerData);
	DeleteContainer2nd(m_mapData);
}

bool BDataPeer::Send(BPeer* primaryPeer, LPCTSTR lpszFile, BDataInfo*& pDI)
{
	pDI = NULL;
	auto* pPDI = BSyncEnsurePointer(m_csPeerData, m_mapPeerData, primaryPeer);
	if (-1 != m_nMaxSend && pPDI->GetTransmittingSize() >= m_nMaxSend)
	{
		pPDI->PushWait(lpszFile, true);
		return true;
	}

	pDI = new BDataInfo();
	int nID = NewID();
	if (!pDI->PrepareForSend(nID, lpszFile))
	{
		SAFE_DELETE(pDI);
		return false;
	}

	pDI->SetPrimaryPeer(primaryPeer);

	BSyncAdd(m_csData, m_mapData, nID, pDI);

	return true;
}

bool BDataPeer::Recv(BPeer* primaryPeer, int nID, unsigned long long cb, LPCTSTR lpszFile, BDataInfo*& pDI)
{
	nID = BDT_RECVID(nID);
	pDI = NULL;
	auto* pPDI = BSyncEnsurePointer(m_csPeerData, m_mapPeerData, primaryPeer);
	if (-1 != m_nMaxRecv && pPDI->GetTransmittingSize() >= m_nMaxRecv)
	{
		pPDI->PushWait(lpszFile, false);
		return true;
	}

	pDI = new BDataInfo();
	if (!pDI->PrepareForRecv(nID, cb, lpszFile))
	{
		SAFE_DELETE(pDI);
		return false;
	}

	pDI->SetPrimaryPeer(primaryPeer);

	BSyncAdd(m_csData, m_mapData, nID, pDI);

	return true;
}

int BDataPeer::NewID()
{
	int nID = 0;
	{
		BScopeLock sl(&m_csData);
		nID = BDT_ID(m_keyRandGen(m_keyRandRng));
		auto iter = m_mapData.find(nID);
		while (m_mapData.end() != iter)
		{
			nID = BDT_ID(m_keyRandGen(m_keyRandRng));
			iter = m_mapData.find(nID);
		}
	}

	return nID;
}

bool BDataPeer::PreTransmit(int nID, BPeer*& peer, BDataInfo** ppDI)
{
	BDataInfo* pDI = BSyncGetPointer(m_csData, m_mapData, nID);
	CHK_false(pDI);

	BPeerDataInfo* pPDI = BSyncGetPointer(m_csPeerData, m_mapPeerData, pDI->GetPrimaryPeer());
	peer = pPDI->PopPeer();
	if (peer)
	{
		// Got Pending, and send NOW

		auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
		pIntern->m_traitVal = pDI;
		pDI->SetPeer(peer);
		pDI->SetTransmitting();
		pPDI->AddTransmitting(pDI);
	}
	else
	{
		// No peers now, so PENDING

		pPDI->AddPending(pDI);
	}

	if (pPDI)
	{
		*ppDI = pDI;
	}
	
	return peer;
}

BDataInfo* BDataPeer::Transmit(int nID, BPeer* peer)
{
	BDataInfo* pDI = BSyncGetPointer(m_csData, m_mapData, nID);
	CHK_NULL(pDI);

	pDI->SetPeer(peer);
	pDI->SetTransmitting();

	BPeerDataInfo* pPDI = BSyncGetPointer(m_csPeerData, m_mapPeerData, pDI->GetPrimaryPeer());
	CHK_NULL(pDI);

	pPDI->GetPending(nID);
	pPDI->AddTransmitting(pDI);

	auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	pIntern->m_traitVal = pDI;

	return pDI;
}

BDataInfo* BDataPeer::TransmitDirectly(int nID, BPeer* peer)
{
	BDataInfo* pDI = BSyncGetPointer(m_csData, m_mapData, nID);
	CHK_NULL(pDI);

	if (pDI->IsTransmitting())
	{
		return pDI;
	}

	BPeerDataInfo* pPDI = BSyncGetPointer(m_csPeerData, m_mapPeerData, pDI->GetPrimaryPeer());
	CHK_NULL(pDI);

	auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	pIntern->m_traitVal = pDI;
	pDI->SetPeer(peer);
	pDI->SetTransmitting();
	pPDI->AddTransmitting(pDI);

	return pDI;
}

bool BDataPeer::PostTransmit(int nID, BPeer*& primaryPeer, BWaitDataInfo*& pWait)
{
	BDataInfo* pDI = BSyncGetPointer2(m_csData, m_mapData, nID);
	CHK_false(pDI);

	BPeerDataInfo* pPDI = BSyncGetPointer(m_csPeerData, m_mapPeerData, pDI->GetPrimaryPeer());
	CHK_false(pPDI);

	pPDI->GetTransmitting(nID);
	BPeer* peer = pDI->GetPeer();
	primaryPeer = pDI->GetPrimaryPeer();
	SAFE_DELETE(pDI);
	pPDI->PushPeer(peer);

	if (pPDI->GetWaitSize())
	{
		pWait = pPDI->PopWait();
		return true;
	}

	return false;
}

void BDataPeer::TerminateTransmit(BPeer* primaryPeer, BPeerDataInfo** ppPDI)
{
	BPeerDataInfo* pPDI = BSyncGetPointer2(m_csPeerData, m_mapPeerData, primaryPeer);
	CHK(pPDI);

	if (ppPDI)
	{
		*ppPDI = pPDI;
	}
	else
	{
		PurgePeerTransmit(pPDI);
	}
}

BDataInfo* BDataPeer::GetDataInfo(int nID)
{
	return BSyncGetPointer(m_csData, m_mapData, nID);
}

void BDataPeer::TerminateTransmit(int nID)
{
	BDataInfo* pDI = BSyncGetPointer2(m_csData, m_mapData, nID);
	CHK(pDI);

	BPeerDataInfo* pPDI = BSyncGetPointer(m_csPeerData, m_mapPeerData, pDI->GetPrimaryPeer());
	CHK(pDI);

	pPDI->GetPending(nID);
	pPDI->GetTransmitting(nID);

	SAFE_DELETE(pDI);
}

void BDataPeer::PurgePeerTransmit(BPeerDataInfo* pPDI)
{
	CHK(pPDI);

	pPDI->ClearWorking();

	list<int> list;

	{
		BScopeLock sl(&pPDI->m_csPending);

		for_each(pPDI->m_mapPending.begin(), pPDI->m_mapPending.end(), [&](pair<int, BDataInfo*> pair)
		{
			list.push_back(pair.first);
		});
	}

	{
		BScopeLock sl(&pPDI->m_csTransmitting);

		for_each(pPDI->m_mapTransmitting.begin(), pPDI->m_mapTransmitting.end(), [&](pair<int, BDataInfo*> pair)
		{
			list.push_back(pair.first);
		});
	}

	for_each(begin(list), end(list), [&](int val)
	{
		auto* pDI = BSyncGetPointer2(m_csData, m_mapData, val);
		SAFE_DELETE(pDI);
	});

	pPDI->SetTerminate(false);
	delete pPDI;
}
