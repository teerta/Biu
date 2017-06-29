
#include "stdafx.h"
#include "bnctxt.h"
#include "bbfunc.h"
#include <cassert>


using namespace std;
using namespace Biu;


// BNServerContext

BNServerContext::BNServerContext()
	: m_pServer(NULL)
	, m_fIsPrimaryService(true)
	, m_pContextFactory(NULL)
{
}

BNServerContext::~BNServerContext()
{
	{
		BScopeLock sl(&m_csRelation);
		DeleteContainer2nd(m_mapRelation);
	}

	{
		BScopeLock sl(&m_csContext);
		DeleteContainer2nd(m_mapPeerContext);
		m_mapUserContext.clear();
	}

	{
		BScopeLock sl(&m_csUserStat);
		DeleteContainer2nd(m_mapUserStat);
	}

	if (m_fIsPrimaryService)
	{
		BScopeLock sl(&m_csUser);
		DeleteContainer2nd(m_mapUser);
	}
}

BNPeerContext* BNServerContext::AddPeer(BPeerRepresentation* pRepresentation, BNUserInfo& userInfo)
{
	assert(pRepresentation);

	BNPeerContext* ppc = GetPeerContext(pRepresentation);
	if (ppc)
	{
		ppc->GetPlatform() = userInfo.GetPlatform();
		return ppc;
	}

	BNUserContext* puc = GetUserContextFromStore(userInfo.GetUID());
	if (!puc)
	{
		puc = AddUser(userInfo.GetUID());
	}

	//CDPUserContext* puc = AddUser(userInfo.m_lUID);

	puc->SetFromUserInfo(userInfo);
	puc->GetRefCount()++;

	ppc = AddPeer(pRepresentation, puc);
	ppc->GetPlatform() = userInfo.GetPlatform();

	return ppc;
}

BNPeerContext* BNServerContext::AddPeer(BPeerRepresentation* pRepresentation, BNUserContext* puserc)
{
	assert(pRepresentation);

	BNPeerContext* ppc = GetPeerContext(pRepresentation);
	if (ppc)
	{
		return ppc;
	}

	ppc = m_pContextFactory->CreatePeerContext();
	ppc->SetRepresentation(pRepresentation);

	{
		BScopeLock sl(&m_csContext);

		m_mapPeerContext[pRepresentation] = ppc;
		m_mapUserContext[pRepresentation] = puserc;
	}

	ppc->GetLoginStatus() = true;

	AddRelation(puserc->GetUID(), pRepresentation);

	StatLogin(puserc->GetUID());

	return ppc;
}


BNUserContext* BNServerContext::UpdatePeer(BPeerRepresentation* pRepresentation, BNUserInfo& userInfo)
{
	assert(pRepresentation);

	BNUserContext* puc = GetUserContextFromStore(userInfo.GetUID());
	if (!puc)
	{
		puc = AddUser(userInfo.GetUID());
	}

	puc->SetFromUserInfo(userInfo);
	puc->GetRefCount()++;

	BNUserContext* pucOld = NULL;
	{
		BScopeLock sl(&m_csContext);

		pucOld = m_mapUserContext[pRepresentation];
		pucOld->GetRefCount()--;
		RemoveRelation(pucOld->GetUID(), pRepresentation);

		m_mapUserContext[pRepresentation] = puc;

		BNPeerContext* ppc = m_mapPeerContext[pRepresentation];
		ppc->GetPlatform() = userInfo.GetPlatform();
		ppc->GetLoginStatus() = true;
		AddRelation(puc->GetUID(), pRepresentation);
	}

	if (pucOld && !pucOld->GetRefCount())
	{
		RemoveUser(pucOld->GetUID());
	}

	return puc;
}

void BNServerContext::RemovePeer(BPeerRepresentation* pRepresentation, BNPeerContext** ppPeer, BNUserContext** ppUser)
{
	assert(pRepresentation);

	BNPeerContext* ppc = GetPeerContext(pRepresentation);
	CHK(ppc);

	ppc->GetLoginStatus() = false;

	BNUserContext* puc = GetUserContext(pRepresentation);
	CHK(puc);

	RemoveRelation(puc->GetUID(), pRepresentation);

	puc->GetRefCount()--;

	{
		BScopeLock sl(&m_csContext);

		m_mapPeerContext.erase(pRepresentation);
		m_mapUserContext.erase(pRepresentation);
	}

	if (ppPeer)
	{
		SAFE_DELETE(*ppPeer);
		*ppPeer = ppc;
	}
	else
	{
		delete ppc;
	}

	if (puc->GetRefCount())
	{
		if (ppUser)
		{
			*ppUser = puc;
		}
	}
	else
	{
		RemoveUser(puc->GetUID(), ppUser);
	}
}

BNPeerContext* BNServerContext::GetPeerContext(BPeerRepresentation* pRepresentation)
{
	assert(pRepresentation);

	BNPeerContext* ppc = NULL;
	{
		BScopeLock sl(&m_csContext);

		auto iter = m_mapPeerContext.find(pRepresentation);
		if (end(m_mapPeerContext) != iter)
		{
			ppc = iter->second;
		}
	}

	return ppc;
}

BNUserContext* BNServerContext::GetUserContext(BPeerRepresentation* pRepresentation)
{
	assert(pRepresentation);

	BNUserContext* puc = NULL;
	{
		BScopeLock sl(&m_csContext);

		auto iter = m_mapUserContext.find(pRepresentation);
		if (end(m_mapUserContext) != iter)
		{
			puc = iter->second;
		}
	}

	return puc;
}

bool BNServerContext::IsPeerLogin(BPeerRepresentation* pRepresentation)
{
	BNPeerContext* ppc = GetPeerContext(pRepresentation);
	CHK_false(ppc);

	return ppc->GetLoginStatus();
}

BNUserContext* BNServerContext::AddUser(int nUID)
{
	CHK_NULL(nUID);

	BNUserContext* puc = m_pContextFactory->CreateUserContext();
	puc->SetUID(nUID);

	{
		BScopeLock sl(&m_csUser);

		m_mapUser[nUID] = puc;
	}

	return puc;
}

void BNServerContext::RemoveUser(int nUID, BNUserContext** ppUser)
{
	BNUserContext* puc = GetUserContextFromStore(nUID);
	if (!puc)
	{
		return;
	}

	{
		BScopeLock sl(&m_csUser);

		m_mapUser.erase(nUID);
	}

	if (ppUser)
	{
		SAFE_DELETE(*ppUser);
		*ppUser = puc;
	}
	else
	{
		delete puc;
	}
}

BNUserContext* BNServerContext::GetUserContextFromStore(int nUID)
{
	CHK_NULL(nUID);

	BNUserContext* puc = NULL;
	{
		BScopeLock sl(&m_csUser);

		auto iter = m_mapUser.find(nUID);
		if (end(m_mapUser) != iter)
		{
			puc = iter->second;
		}
	}

	return puc;
}

bool BNServerContext::IsUserExist(int nUID)
{
	bool bRet = false;
	{
		BScopeLock sl(&m_csUser);

		auto iter = m_mapUser.find(nUID);
		if (end(m_mapUser) != iter)
		{
			bRet = true;
		}
	}

	return bRet;
}

bool BNServerContext::GetRelation(int nUID, list<BPeerRepresentation*>& pRepresentations)
{
	pRepresentations.clear();
	{
		BScopeLock sl(&m_csRelation);

		auto iter = m_mapRelation.find(nUID);
		if (end(m_mapRelation) != iter)
		{
			auto l = iter->second;
			pRepresentations.insert(begin(pRepresentations), l->begin(), l->end());
		}
	}

	return !pRepresentations.empty();
}

bool BNServerContext::HasRelation(int nUID)
{
	bool bRet = false;
	{
		BScopeLock sl(&m_csRelation);

		auto iter = m_mapRelation.find(nUID);
		if (end(m_mapRelation) != iter)
		{
			if (iter->second && !iter->second->empty())
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

void BNServerContext::AddRelation(int nUID, BPeerRepresentation* pRepresentation)
{
	BScopeLock sl(&m_csRelation);

	auto iter = m_mapRelation.find(nUID);
	if (end(m_mapRelation) != iter)
	{
		auto pRelation = iter->second;
		if (pRelation->end() == find(pRelation->cbegin(), pRelation->cend(), pRepresentation))
		{
			pRelation->push_back(pRepresentation);
		}
	}
	else
	{
		auto pList = new list<BPeerRepresentation*>();
		pList->push_back(pRepresentation);
		m_mapRelation[nUID] = pList;
	}
}

void BNServerContext::RemoveRelation(int nUID, BPeerRepresentation* pRepresentation)
{
	BScopeLock sl(&m_csRelation);

	auto iter = m_mapRelation.find(nUID);
	if (end(m_mapRelation) != iter)
	{
		auto pRelation = iter->second;
		auto iterRelation = find(pRelation->begin(), pRelation->end(), pRepresentation);
		if (pRelation->end() != iterRelation)
		{
			pRelation->erase(iterRelation);
		}
	}
}

bool BNServerContext::IsEmpty()
{
	bool bRet = false;
	{
		BScopeLock sl(&m_csContext);

		bRet = m_mapPeerContext.empty();
	}

	return bRet;
}

bool BNServerContext::HasUserAppeared(int nUID)
{
	bool bRet = false;
	{
		BScopeLock sl(&m_csUserStat);

		auto iter = m_mapUserStat.find(nUID);
		if (end(m_mapUserStat) != iter)
		{
			bRet = true;
		}
	}

	return bRet;
}

void BNServerContext::StatLogin(int nUID)
{
	BNUserStatistics* pStat = NULL;
	{
		BScopeLock sl(&m_csUserStat);

		auto iter = m_mapUserStat.find(nUID);
		if (end(m_mapUserStat) == iter)
		{
			pStat = new BNUserStatistics();
			m_mapUserStat[nUID] = pStat;
		}
		else
		{
			pStat = iter->second;
		}
	}

	pStat->IncLogin();
}
