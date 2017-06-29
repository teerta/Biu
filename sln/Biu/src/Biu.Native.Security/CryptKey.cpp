
#include "stdafx.h"
#include "CryptKey.h"
#include "KeyBlob.h"


using namespace Biu::Native::Security;


CryptKey::CryptKey()
	: m_pProv(NULL)
	, m_hKey(NULL)
{
}


CryptKey::~CryptKey()
{
	this->!CryptKey();
}


CryptKey::!CryptKey()
{
	CHK(m_pProv);

	if (m_hKey)
	{
		CryptDestroyKey(m_hKey);
		m_hKey = NULL;
	}
}


KeyBlob^ CryptKey::ExportSessionKey(CryptKey^ encKey, bool keepKeyForBlob)
{
	if (!m_pProv || !m_hKey)
	{
		return nullptr;
	}

	auto blob = gcnew KeyBlob();
	LPBYTE lp = blob->m_lpKey;
	blob->m_cbKey = m_pProv->ExportSessionKey(m_hKey, nullptr == encKey ? NULL : encKey->m_hKey, &lp);
	if (0 == blob->m_cbKey)
	{
		blob = nullptr;
	}
	else
	{
		if (nullptr != encKey)
		{
			blob->IsEncrypted = true;
			if (keepKeyForBlob)
			{
				blob->EncryptionKey = encKey;
			}
		}
	}

	return blob;
}
