
#include "stdafx.h"
#include "bsec.h"
#include "bmarco.h"
#include "bcp.h"
#include "bp4.h"
#include "bp4s.h"
#include "bexception.h"


using namespace Biu;


// BSecurityItem

BSecurityItem::BSecurityItem(Alg alg, ALG_ID algID, DWORD dwFlags)
	: m_alg(alg)
	, m_pNext(NULL)
	, m_pProv(NULL)
	, m_pAlgP4(NULL)
	, m_pAlgP4S(NULL)
	, m_lRef(0)
	, m_hKey(NULL)
{
	try
	{
		m_pProv = new BCryptProvider();
		HCRYPTKEY hKey = m_pProv->GenerateKey(algID, dwFlags);
		if (!hKey)
		{
			throw BE_InitObject("Failed: Generate Key");
		}
		
		SetKey(hKey);
		if (Alg_P4 & alg)
		{
			m_pAlgP4 = new BP4();
			m_pAlgP4->SetProvider(m_pProv);
			m_pAlgP4->SetHashLen(m_pProv->GetHashResultLength(CALG_SHA_256));
			m_pAlgP4->SetSigLen(m_pProv->GetHashSignatureLength(CALG_SHA_256));
		}
		else if (Alg_P4S & alg)
		{
			m_pAlgP4S = new BP4S();
			m_pAlgP4S->SetProvider(m_pProv);
			m_pAlgP4S->SetHashLen(m_pProv->GetHashResultLength(CALG_SHA_256));
			m_pAlgP4S->SetSigLen(m_pProv->GetHashSignatureLength(CALG_SHA_256));
			m_pAlgP4S->SetKeyLen(m_pProv->GetKeyLength(CALG_AES_256, CRYPT_EXPORTABLE, hKey));
		}
	}
	catch (...)
	{
		throw;
	}	
}

BSecurityItem::~BSecurityItem()
{
	SAFE_DELETE(m_pAlgP4);
	SAFE_DELETE(m_pAlgP4S);
	if (m_hKey)
	{
		CryptDestroyKey(m_hKey);
	}

	SAFE_DELETE(m_pProv);
}

BSecurityItem* BSecurityItem::GetLastItem()
{
	BSecurityItem* pItem = this;
	while (pItem)
	{
		if (!pItem->m_pNext)
		{
			break;
		}

		pItem = pItem->m_pNext;
	}

	return pItem;
}
