
#include "stdafx.h"
#include "bpeerdata.h"
#include "bbfunc.h"
#include "priv/i_peer.h"

#include <cassert>
#include <algorithm>


using namespace std;
using namespace Biu;


// BClientDataPeer

BClientDataPeer::BClientDataPeer(BPeer* peer)
	: m_pPDI(NULL)
	, m_primaryPeer(peer)
{
	m_nMaxSend = -1;
	m_nMaxRecv = -1;

	m_pPDI = new BPeerDataInfo();
}

BClientDataPeer::~BClientDataPeer()
{
	SAFE_DELETE(m_pPDI);
	DeleteContainer2nd(m_mapData);
}

bool BClientDataPeer::Send(LPCTSTR lpszFile, BDataInfo*& pDI)
{
	pDI = NULL;
	if (-1 != m_nMaxSend && m_pPDI->GetTransmittingSize() >= m_nMaxSend)
	{
		m_pPDI->PushWait(lpszFile, true);
		return true;
	}

	pDI = new BDataInfo();
	int nID = NewID();
	if (!pDI->PrepareForSend(nID, lpszFile))
	{
		SAFE_DELETE(pDI);
		return false;
	}

	pDI->SetPrimaryPeer(m_primaryPeer);

	BSyncAdd(m_csData, m_mapData, nID, pDI);

	return true;
}

bool BClientDataPeer::Recv(int nID, unsigned long long cb, LPCTSTR lpszFile, BDataInfo*& pDI)
{
	nID = BDT_RECVID(nID);
	pDI = NULL;
	if (-1 != m_nMaxRecv && m_pPDI->GetTransmittingSize() >= m_nMaxRecv)
	{
		m_pPDI->PushWait(lpszFile, false);
		return true;
	}

	pDI = new BDataInfo();
	if (!pDI->PrepareForRecv(nID, cb, lpszFile))
	{
		SAFE_DELETE(pDI);
		return false;
	}

	pDI->SetPrimaryPeer(m_primaryPeer);

	BSyncAdd(m_csData, m_mapData, nID, pDI);

	return true;
}

int BClientDataPeer::NewID()
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

bool BClientDataPeer::PreTransmit(int nID, BPeer*& peer, BDataInfo** ppDI)
{
	BDataInfo* pDI = BSyncGetPointer(m_csData, m_mapData, nID);
	CHK_false(pDI);

	peer = m_pPDI->PopPeer();
	if (peer)
	{
		// Got Pending, and send NOW

		auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
		pIntern->m_traitVal = pDI;
		pDI->SetPeer(peer);
		pDI->SetTransmitting();
		m_pPDI->AddTransmitting(pDI);
	}
	else
	{
		// No peers now, so PENDING

		m_pPDI->AddPending(pDI);
	}

	if (ppDI)
	{
		*ppDI = pDI;
	}

	return peer;
}

BDataInfo* BClientDataPeer::Transmit(int nID, BPeer* peer)
{
	BDataInfo* pDI = BSyncGetPointer(m_csData, m_mapData, nID);
	CHK_NULL(pDI);

	pDI->SetPeer(peer);
	pDI->SetTransmitting();

	m_pPDI->GetPending(nID);
	m_pPDI->AddTransmitting(pDI);

	auto* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	pIntern->m_traitVal = pDI;

	return pDI;
}

bool BClientDataPeer::PostTransmit(int nID, BPeer*& primaryPeer, BWaitDataInfo*& pWait)
{
	BDataInfo* pDI = BSyncGetPointer2(m_csData, m_mapData, nID);
	CHK_false(pDI);	

	m_pPDI->GetTransmitting(nID);
	BPeer* peer = pDI->GetPeer();
	primaryPeer = pDI->GetPrimaryPeer();
	SAFE_DELETE(pDI);
	m_pPDI->PushPeer(peer);

	if (m_pPDI->GetWaitSize())
	{
		pWait = m_pPDI->PopWait();
		return true;
	}

	return false;
}

BDataInfo* BClientDataPeer::GetDataInfo(int nID)
{
	return BSyncGetPointer(m_csData, m_mapData, nID);
}

void BClientDataPeer::TerminateTransmit(int nID)
{
	BDataInfo* pDI = BSyncGetPointer2(m_csData, m_mapData, nID);
	CHK(pDI);

	m_pPDI->GetPending(nID);
	m_pPDI->GetTransmitting(nID);

	SAFE_DELETE(pDI);
}
