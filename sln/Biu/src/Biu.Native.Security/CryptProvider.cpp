
#include "stdafx.h"
#include "CryptProvider.h"
#include "CryptKey.h"
#include "KeyBlob.h"


using namespace Biu::Native::Security;


CryptProvider::CryptProvider()
	: m_pProv(NULL)
{
	m_pProv = new BCryptProvider();
}


CryptProvider::~CryptProvider()
{
	this->!CryptProvider();
}


CryptProvider::!CryptProvider()
{
	SAFE_DELETE(m_pProv);
}


CryptKey^ CryptProvider::GenerateAES256Key()
{
	HCRYPTKEY hKey = m_pProv->GenerateKey(CALG_AES_256, CRYPT_EXPORTABLE);
	if (!hKey)
	{
		return nullptr;
	}

	auto key = gcnew CryptKey();
	key->m_pProv = m_pProv;
	key->m_hKey = hKey;

	return key;
}


KeyBlob^ CryptProvider::GenerateAES256Blob()
{
	static const DWORD Size_Of_Plain_Key = 512;

	auto blob = gcnew KeyBlob(Size_Of_Plain_Key);
	auto rand = gcnew Random();
	for (size_t i = 0; i < Size_Of_Plain_Key; i++)
	{
		blob->m_lpKey[i] = (Byte)rand->Next(256);
	}

	return blob;
}


CryptKey^ CryptProvider::ImportSessionKey(KeyBlob^ blob)
{
	if (blob->IsEncrypted && nullptr == blob->EncryptionKey)
	{
		throw gcnew ArgumentException("blob\'s EncryptionKey is null as it is encrypted");
	}

	LPBYTE lp = blob->m_lpKey;
	HCRYPTKEY hKey = m_pProv->ImportEncryptedKey(&lp, blob->m_cbKey, nullptr == blob->EncryptionKey ? NULL : blob->EncryptionKey->m_hKey, false);
	if (!hKey)
	{
		return nullptr;
	}

	auto key = gcnew CryptKey();
	key->m_pProv = m_pProv;
	key->m_hKey = hKey;

	return key;
}


CryptKey^ CryptProvider::ImportBlobSessiobKey(KeyBlob^ blob, ALG_ID algHash, ALG_ID algKey, DWORD dwFlag)
{
	if (nullptr == blob)
	{
		throw gcnew ArgumentNullException(_T("blob is null"));
	}

	HCRYPTKEY hKey = m_pProv->DeriveKey(blob->m_lpKey, blob->m_cbKey, algHash, algKey, dwFlag);
	if (!hKey)
	{
		return nullptr;
	}

	auto key = gcnew CryptKey();
	key->m_pProv = m_pProv;
	key->m_hKey = hKey;

	return key;
}
