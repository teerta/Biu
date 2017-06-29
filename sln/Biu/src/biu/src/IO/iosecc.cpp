
#include "stdafx.h"
#include "biosecc.h"
#include "bp4.h"
#include "bexception.h"


using namespace Biu;


BIOSecurityContext::BIOSecurityContext(BSecurityProvider* pProv)
	: m_pProv(NULL)
	, m_pLastItem(NULL)
	, m_hKeyRemote(NULL)
{
	m_pProv = pProv;
	try
	{
		Create();
	}
	catch (...)
	{
		throw;
	}
}

BIOSecurityContext::~BIOSecurityContext()
{
	if (m_hKeyRemote)
	{
		CryptDestroyKey(m_hKeyRemote);
		m_hKeyRemote = NULL;
	}
}

bool BIOSecurityContext::ChangeRemoteKey(LPBYTE lpKey, DWORD cbKey)
{
	CHK_false(Create());

	m_remoteKeyRaw.Set(lpKey, cbKey);
	if (m_hKeyRemote)
	{
		CryptDestroyKey(m_hKeyRemote);
		m_hKeyRemote = NULL;
	}

	m_hKeyRemote = m_pLastItem->GetCryptProvider()->ImportKey(&m_remoteKeyRaw.data, m_remoteKeyRaw.cb, false);
	if (!m_hKeyRemote)
	{
		return false;
	}

	return true;
}

BSecurityItem* BIOSecurityContext::GetSecurityItem()
{
	CHK_NULL(Create());
	return m_pLastItem;
}

bool BIOSecurityContext::Create()
{
	auto* p = m_pProv->GetItem(m_pLastItem);
	if (m_pLastItem)
	{
		if (m_pLastItem != p)
		{
			if (m_hKeyRemote)
			{
				CryptDestroyKey(m_hKeyRemote);
				m_hKeyRemote = NULL;
			}

			if (m_remoteKeyRaw.HasData())
			{
				m_hKeyRemote = p->GetCryptProvider()->ImportKey(&m_remoteKeyRaw.data, m_remoteKeyRaw.cb, false);
				if (!m_hKeyRemote)
				{
					return false;
				}
			}

			m_pLastItem = p;
		}
	}
	else
	{
		m_pLastItem = p;
	}

	return true;
}

bool BIOSecurityContext::ChangeRemoteKeyStatic(LPBYTE lpKey, DWORD cbKey)
{
	m_remoteKeyRaw.Set(lpKey, cbKey);
	if (m_hKeyRemote)
	{
		CryptDestroyKey(m_hKeyRemote);
		m_hKeyRemote = NULL;
	}

	m_hKeyRemote = m_pLastItem->GetCryptProvider()->ImportKey(&m_remoteKeyRaw.data, m_remoteKeyRaw.cb, false);
	if (!m_hKeyRemote)
	{
		return false;
	}

	return true;
}
