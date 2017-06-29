
#include "stdafx.h"
#include "bsec.h"
#include "bmarco.h"
#include "bcp.h"


using namespace Biu;


// BSecurityProvider

BSecurityProvider::BSecurityProvider()
	: m_pItem(NULL)
	, m_lItem(0)
{

}

BSecurityProvider::~BSecurityProvider()
{
	while (m_pItem)
	{
		auto* p = m_pItem->GetNext();
		delete m_pItem;
		m_pItem = p;
	}

	m_pItem = NULL;
}

BSecurityItem* BSecurityProvider::GetItem(BSecurityItem* pLastItem)
{
	BSecurityItem* pItem = pLastItem;
	while (InterlockedCompareExchange(&m_lItem, 0, 0))
	{
		Sleep(100);
	}

	CHK_NULL(m_pItem);

	auto* pNewLastItem = m_pItem->GetLastItem();
	if (!pLastItem)
	{
		pItem = pNewLastItem;
		pItem->AddRef();
	}
	else if (pLastItem != pNewLastItem)
	{
		pItem = pNewLastItem;
		if (!pLastItem->Release())
		{
			DeleteItem(pLastItem);
		}
	}

	return pItem;
}

BSecurityItem* BSecurityProvider::NewItem(BSecurityItem::Alg alg)
{
	auto* p = new BSecurityItem(alg, AT_KEYEXCHANGE, CRYPT_EXPORTABLE);
	EnsureItem(p);

	return p;
}

void BSecurityProvider::EnsureItem(BSecurityItem* pItem)
{
	BScopeLock sl(&m_csItem);
	InterlockedExchange(&m_lItem, 1);

	if (m_pItem)
	{
		if (m_pItem->GetRefCount())
		{
			auto* pTemp = m_pItem;
			m_pItem = m_pItem->GetNext();
			delete pTemp;
			EnsureItem(pItem);
		}
		else
		{
			m_pItem->GetLastItem()->SetNext(pItem);
		}
	}
	else
	{
		m_pItem = pItem;
	}

	InterlockedExchange(&m_lItem, 0);
}

void BSecurityProvider::DeleteItem(BSecurityItem* pItem)
{
	CHK(pItem);

	BScopeLock sl(&m_csItem);
	InterlockedExchange(&m_lItem, 1);

	BSecurityItem* p1 = NULL;
	BSecurityItem* p2 = m_pItem;
	while (p2)
	{
		if (p2 == pItem)
		{
			if (p1)
			{
				m_pItem = pItem->GetNext();
			}
			else
			{
				p1->SetNext(pItem->GetNext());
			}
			
			delete pItem;
			break;
		}

		p1 = p2;
		p2 = p2->GetNext();
	}

	InterlockedExchange(&m_lItem, 0);
}
