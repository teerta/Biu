
#include "stdafx.h"
#include "bcp.h"
#include "bexception.h"
#include "bmarco.h"


using namespace Biu;


// BCryptPtovider

BCryptProvider::BCryptProvider(LPCWSTR szProvider, DWORD dwProvType)
	: m_hProv(NULL)
{
	if (!CryptAcquireContext(&m_hProv, NULL, szProvider, dwProvType, CRYPT_VERIFYCONTEXT))
	{
		throw BE_InitObject("Failes: Create BCryptProvider");
	}
}

BCryptProvider::~BCryptProvider()
{
	if (m_hProv)
	{
		CryptReleaseContext(m_hProv, 0);
		m_hProv = NULL;
	}
}

HCRYPTKEY BCryptProvider::GenerateKey(ALG_ID algID, DWORD dwFlags)
{
	HCRYPTKEY hKey = NULL;
	CryptGenKey(m_hProv, algID, dwFlags, &hKey);

	return hKey;
}

DWORD BCryptProvider::ExportKey(HCRYPTKEY hKey, LPBYTE* lppKeyBlob)
{
	assert(hKey);
	assert(lppKeyBlob);

	DWORD cb = 0;
	if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, NULL, &cb))
	{
		return 0;
	}

	*lppKeyBlob = (LPBYTE)malloc(cb);

	return CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, *lppKeyBlob, &cb) ? cb : 0;
}

DWORD BCryptProvider::ExportKeyPair(HCRYPTKEY hKey, HCRYPTKEY hExpKey, LPBYTE* lppKeyBlob)
{
	assert(hKey);
	assert(hExpKey);
	assert(lppKeyBlob);

	DWORD cb = 0;
	if (!CryptExportKey(hKey, hExpKey, PRIVATEKEYBLOB, 0, NULL, &cb))
	{
		return 0;
	}

	*lppKeyBlob = (LPBYTE)malloc(cb);

	return CryptExportKey(hKey, hExpKey, PRIVATEKEYBLOB, 0, *lppKeyBlob, &cb) ? cb : 0;
}

DWORD BCryptProvider::ExportSessionKey(HCRYPTKEY hKey, HCRYPTKEY hExpKey, LPBYTE* lppKeyBlob)
{
	assert(hKey);
	assert(lppKeyBlob);

	DWORD cb = 0;
	if (!CryptExportKey(hKey, hExpKey, SIMPLEBLOB, 0, NULL, &cb))
	{
		return 0;
	}

	*lppKeyBlob = (LPBYTE)malloc(cb);

	return CryptExportKey(hKey, hExpKey, SIMPLEBLOB, 0, *lppKeyBlob, &cb) ? cb : 0;
}

HCRYPTKEY BCryptProvider::ImportKey(LPBYTE* lppKeyBlob, DWORD cbKeyBlob, bool bDeleteBlob)
{
	assert(lppKeyBlob);
	assert(cbKeyBlob);

	HCRYPTKEY hKey = NULL;
	if (!CryptImportKey(m_hProv, *lppKeyBlob, cbKeyBlob, 0, 0, &hKey))
	{
		return NULL;
	}

	if (bDeleteBlob)
	{
		delete *lppKeyBlob;
		*lppKeyBlob = NULL;
	}

	return hKey;
}

HCRYPTKEY BCryptProvider::ImportEncryptedKey(LPBYTE* lppKeyBlob, DWORD cbKeyBlob, HCRYPTKEY hPubKey, bool bDeleteBlob)
{
	assert(lppKeyBlob);
	assert(cbKeyBlob);
	assert(hPubKey);

	HCRYPTKEY hKey = NULL;
	if (!CryptImportKey(m_hProv, *lppKeyBlob, cbKeyBlob, hPubKey, 0, &hKey))
	{
		return NULL;
	}

	if (bDeleteBlob)
	{
		delete *lppKeyBlob;
		*lppKeyBlob = NULL;
	}

	return hKey;
}

HCRYPTKEY BCryptProvider::DeriveKey(LPBYTE lpKeyBlob, DWORD cbKeyBlob, ALG_ID algHash, ALG_ID algKey, DWORD dwFlag)
{
	assert(lpKeyBlob);
	assert(cbKeyBlob);
	assert(algHash);
	assert(algKey);
	assert(dwFlag);

	bool bSucc = false;
	HCRYPTKEY hKey = NULL;
	HCRYPTHASH hHash = NULL;
	
	__try
	{
		LEAVE(CryptCreateHash(m_hProv, algHash, 0, 0, &hHash));
		LEAVE(CryptHashData(hHash, lpKeyBlob, cbKeyBlob, 0));
		LEAVE(CryptDeriveKey(m_hProv, algKey, hHash, dwFlag, &hKey));

		bSucc = true;
	}
	__finally
	{
		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}

		if (!bSucc)
		{
			if (hKey)
			{
				CryptDestroyKey(hKey);
				hKey = NULL;
			}
		}
	}

	return hKey;
}

DWORD BCryptProvider::GetHashResultLength(ALG_ID algID)
{
	const DWORD Size_Of_data = 1024;

	HCRYPTHASH hHash = NULL;
	DWORD cbData = Size_Of_data;
	BYTE rgData[Size_Of_data] = { 0 };
	DWORD dwHashLen = 0;
	DWORD cbHashLen = sizeof(DWORD);

	__try
	{
		if (!CryptCreateHash(m_hProv, algID, 0, 0, &hHash))
		{
			__leave;
		}

		if (!CryptGenRandom(m_hProv, cbData, rgData))
		{
			__leave;
		}

		if (!CryptHashData(hHash, rgData, cbData, 0))
		{
			__leave;
		}

		if (!CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&dwHashLen, &cbHashLen, 0))
		{
			__leave;
		}
	}
	__finally
	{
		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}
	}

	return dwHashLen;
}

DWORD BCryptProvider::GetHashSignatureLength(ALG_ID algID)
{
	const DWORD Size_Of_data = 1024;

	HCRYPTHASH hHash = NULL;
	DWORD cbData = Size_Of_data;
	BYTE rgData[Size_Of_data] = { 0 };
	DWORD cbHash = 0;
	LPBYTE lpSig = NULL;
	DWORD cbSig = 0;

	__try
	{
		if (!CryptCreateHash(m_hProv, algID, 0, 0, &hHash))
		{
			__leave;
		}

		if (!CryptGenRandom(m_hProv, cbData, rgData))
		{
			__leave;
		}

		if (!CryptHashData(hHash, rgData, cbData, 0))
		{
			__leave;
		}

		if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, NULL, &cbSig))
		{
			__leave;
		}

		lpSig = (LPBYTE)malloc(cbSig);
		if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, lpSig, &cbSig))
		{
			__leave;
		}
	}
	__finally
	{
		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}

		if (lpSig)
		{
			free(lpSig);
			lpSig = NULL;
		}
	}

	return cbSig;
}

DWORD BCryptProvider::GetKeyLength(ALG_ID algID, DWORD dwFlags, HCRYPTKEY hExpKey)
{
	HCRYPTKEY hKey = GenerateKey(algID, dwFlags);
	if (!hKey)
	{
		return 0;
	}

	DWORD cb = 0;
	CryptExportKey(hKey, hExpKey, SIMPLEBLOB, 0, NULL, &cb);
	CryptDestroyKey(hKey);

	return cb;
}
