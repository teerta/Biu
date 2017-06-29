
#include "stdafx.h"
#include "bnctxt.h"
#include "bbfunc.h"
#include <cassert>
#include <iterator>


using namespace std;
using namespace Biu;


// Class BNHostContext

BNHostContext::BNHostContext()
	: m_pContextFactory(NULL)
{
}

BNHostContext::~BNHostContext()
{
	DeleteContainer2nd(m_mapServer);
	DeleteContainer2nd(m_mapClient);
}

BNServerContext* BNHostContext::operator[](const unsigned int nCookie)
{
	return GetServerContext(nCookie);
}

BNServerContext* BNHostContext::AddServer(BNServiceFactory* pFactory, const unsigned int nCookie, BServerPeer* pServer, const unsigned int nRefPrimaryCookie)
{
	assert(nCookie);
	assert(pServer);

	BNServerContext* psc = GetServerContext(nCookie);
	if (psc)
	{
		return psc;
	}

	psc = m_pContextFactory->CreateServerContext();
	psc->SetServerPeer(pServer);
	psc->SetContextFactory(pFactory);

	{
		BScopeLock sl(&m_csServer);

		m_mapServer[nCookie] = psc;
		m_mapServerMapping[nCookie] = 0 == nRefPrimaryCookie ? nCookie : nRefPrimaryCookie;
	}

	return psc;
}

BNServerContext* BNHostContext::RemoveServer(const unsigned int nCookie)
{
	BNServerContext* psc = GetServerContext(nCookie);
	if (!psc)
	{
		return NULL;
	}

	bool bOK = true;
	{
		BScopeLock sl(&m_csServer);

		if (psc->IsPrimaryService())
		{
			bOK = !any_of(
				m_mapServerMapping.begin(), m_mapServerMapping.end(),
				[&](ServerMappingPair pa)
			{
				return pa.second == nCookie;
			}
			);
		}

		if (bOK)
		{
			m_mapServer.erase(nCookie);
			m_mapServerMapping.erase(nCookie);
		}
		else
		{
			psc = NULL;
		}
	}

	return psc;
}

void BNHostContext::RemoveAllServers()
{
	list<unsigned int> listService;
	{
		BScopeLock sl(&m_csServer);

		transform(begin(m_mapServer), end(m_mapServer), back_inserter(listService),
			[](ServerPair pa)
		{
			return pa.first;
		}
		);
	}

	for_each(begin(listService), end(listService),
		[&](unsigned int nKey)
	{
		RemoveServer(nKey);
	}
	);
}

const unsigned int BNHostContext::GetPrimaryCookie(const unsigned int nCookie)
{
	unsigned int nRet = 0;
	{
		BScopeLock sl(&m_csServer);

		auto iter = m_mapServerMapping.find(nCookie);
		if (end(m_mapServerMapping) != iter)
		{
			nRet = iter->second;
		}
	}

	return nRet;
}

BNServerContext* BNHostContext::GetServerContext(const unsigned int nCookie)
{
	CHK_NULL(nCookie);
	BNServerContext* psc = NULL;
	{
		BScopeLock sl(&m_csServer);
		auto iter = m_mapServer.find(nCookie);
		if (end(m_mapServer) != iter)
		{
			psc = iter->second;
		}
	}

	return psc;
}

BNPeerContext* BNHostContext::AddServerUserFromPrimary(const unsigned int nCookie, BPeerRepresentation* pRepresentation, int nUID)
{
	BNServerContext* pscRef = GetServerContext(GetPrimaryCookie(nCookie));
	CHK_NULL(pscRef);

	BNUserContext* puc = pscRef->GetUserContextFromStore(nUID);
	CHK_NULL(puc);

	BNServerContext* psc = GetServerContext(nCookie);
	CHK_NULL(psc);

	return psc->AddPeer(pRepresentation, puc);
}

BNClientContext* BNHostContext::AddClient(BNServiceFactory* pFactory, BPeer* peer)
{
	assert(peer);

	BNClientContext* pcc = GetClientContext(peer->GetRepresentation());
	if (pcc)
	{
		return pcc;
	}

	pcc = m_pContextFactory->CreateClientContext();
	pcc->SetContextFactory(pFactory);
	pcc->SetRepresentation(peer->GetRepresentation());

	{
		BScopeLock sl(&m_csClient);
		m_mapClient[peer->GetRepresentation()] = pcc;
	}

	return pcc;
}

void BNHostContext::RemoveClient(BPeerRepresentation* pRepresentation)
{
	BNClientContext* pcc = GetClientContext(pRepresentation);
	if (!pcc)
	{
		return;
	}

	{
		BScopeLock sl(&m_csServer);
		m_mapClient.erase(pRepresentation);
	}

	delete pcc;
}

void BNHostContext::RemoveAllClients()
{
	list<BPeerRepresentation*> list;
	{
		BScopeLock sl(&m_csClient);
		transform(begin(m_mapClient), end(m_mapClient), back_inserter(list),
			[](ClientPair pa)
		{
			return pa.first;
		}
		);
	}

	BScopeLock sl(&m_csClient);
	for_each(begin(list), end(list),
		[&](BPeerRepresentation* pRepresentation)
	{
		auto* p = m_mapClient[pRepresentation];
		m_mapClient.erase(pRepresentation);
		delete p;
	});
}

BNClientContext* BNHostContext::GetClientContext(BPeerRepresentation* pRepresentation)
{
	CHK_NULL(pRepresentation);
	BNClientContext* psc = NULL;
	{
		BScopeLock sl(&m_csClient);
		auto iter = m_mapClient.find(pRepresentation);
		if (end(m_mapClient) != iter)
		{
			psc = iter->second;
		}
	}

	return psc;
}
