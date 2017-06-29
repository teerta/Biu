
#include "stdafx.h"
#include "bpeerdata.h"
#include "bbfunc.h"
#include "priv/i_peer.h"

#include <cassert>
#include <algorithm>


using namespace std;
using namespace Biu;


// BPeerDataInfo

BPeerDataInfo::BPeerDataInfo()
	: m_bTerminate(true)
{

}

BPeerDataInfo::~BPeerDataInfo()
{
	DeleteContainer(m_listWait);
	DestructContainer(m_listPeer, [&](BPeer* peer)
	{
		if (m_bTerminate)
		{
			if (peer->GetID())
			{
				if (peer->GetServerID())
				{
					peer->GetServer()->RemovePeer(peer->GetRemoteRepresentation());
				}

				SAFE_DELETE(peer);
			}			
		}
		else
		{
			reinterpret_cast<I_Peer*>(peer->GetIntern())->Destroy();
		}		
	});
}

void BPeerDataInfo::AddPending(BDataInfo* pDI)
{
	BSyncAdd(m_csPending, m_mapPending, pDI->GetID(), pDI);
}

BDataInfo* BPeerDataInfo::GetPending(int nID, bool bRemove)
{
	return BSyncGetPointer2(m_csPending, m_mapPending, nID, bRemove);
}

void BPeerDataInfo::PushPeer(BPeer* peer)
{
	BSyncPushBack(m_csPeer, m_listPeer, peer);
}

BPeer* BPeerDataInfo::PopPeer()
{
	return BSyncPopPointer(m_csPeer, m_listPeer);
}

void BPeerDataInfo::AddTransmitting(BDataInfo* pDI)
{
	BSyncAdd(m_csTransmitting, m_mapTransmitting, pDI->GetID(), pDI);
}

BDataInfo* BPeerDataInfo::GetTransmitting(int nID, bool bRemove)
{
	return BSyncGetPointer2(m_csTransmitting, m_mapTransmitting, nID, bRemove);
}

size_t BPeerDataInfo::GetTransmittingSize()
{
	return BSyncGetSize(m_csTransmitting, m_mapTransmitting);
}

void BPeerDataInfo::PushWait(LPCTSTR lpszFile, bool bDir)
{
	auto* p = new BWaitDataInfo();
	p->SetFile(lpszFile);
	p->SetDirection(bDir);
	BSyncPushBack(m_csWait, m_listWait, p);
}

BWaitDataInfo* BPeerDataInfo::PopWait()
{
	return BSyncPopPointer(m_csWait, m_listWait);
}

size_t BPeerDataInfo::GetWaitSize()
{
	return BSyncGetSize(m_csWait, m_listWait);
}

void BPeerDataInfo::ClearWorking()
{
	BScopeLock sl(&m_csPeer);

	DestructContainer(m_listPeer, [](BPeer* peer)
	{
		reinterpret_cast<I_Peer*>(peer->GetIntern())->Destroy();
	});

	//{
	//	BScopeLock sl(&m_csPending);

	//	m_mapPending.clear();
	//}

	//{
	//	BScopeLock ls(&m_csTransmitting);

	//	m_mapTransmitting.clear();
	//}
}
