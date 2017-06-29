
#include "stdafx.h"
#include "bp4s.h"
#include "bcp.h"
#include "bbase.h"


using namespace Biu;


#define B_RSA_1024_LEN					128
#define B_RSA_2048_LEN					256
#define B_AES_256_LEN					32
#define B_PKCS_1_PADDING_MIN_LEN		11
#define ENCRYPT_BLOCK_LEN				8


// BP4S

BP4S::BP4S(DWORD dwBatch)
	: m_pProv(NULL)
	, m_dwBatch(dwBatch)
	, m_dwHashLen(0)
	, m_dwSigLen(0)
	, m_dwKeyLen(0)
{

}

BP4S::~BP4S()
{

}

/*
	. Generate random session key (per encryption)
	. SHA256-Hash session key
	. AES-256 for Data encryption by session key
	. SHA256-Hash data
	. Sign hash by exchange key
	. RSA for session key by remote exchange public key	
	. SHA256-Hash all data: Signed Hash + Encrypted Session Key + Encrypted Data
*/
bool BP4S::Encrypt(LPBP4SPARAM lpParam)
{
	assert(lpParam);

	BMemoryBrick* pBrick = lpParam->pBrick;
	BMemoryBrick* pNewBrick = lpParam->pShack->Pop();
	HCRYPTPROV hProv = m_pProv->GetProvider();
	bool bRet = false;
	HCRYPTHASH hHash = NULL;
	size_t nPos = lpParam->dwPos1;
	DWORD cbDataSize = B_Size_Of_DWORD;
	HCRYPTKEY hKeyAES = NULL;
	LPBYTE lpBatchBuff = NULL;
	DWORD cbBatch = 0;
	DWORD cbBatchBuff = 0;

	__try
	{
		// Generate session key
		hKeyAES = m_pProv->GenerateKey(CALG_AES_256, CRYPT_EXPORTABLE);
		LEAVE(hKeyAES);


		// Hash session key

		LEAVE(CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash));
		
		LEAVE(CryptHashSessionKey(hHash, hKeyAES, 0));


		// Encrypt & Hash data

		cbBatch = m_dwBatch - m_dwBatch % ENCRYPT_BLOCK_LEN;
		if (ENCRYPT_BLOCK_LEN > 1)
		{
			cbBatchBuff = cbBatch + ENCRYPT_BLOCK_LEN;
		}
		else
		{
			cbBatchBuff = cbBatch;
		}

		lpBatchBuff = new BYTE[cbBatchBuff];

		DWORD cbTotal = pBrick->UntilGetDataSize();
		DWORD cb = 0;
		BOOL bEOS = FALSE;
		do
		{
			cb = BReadBrickSparselyUnchecked_char(pBrick, reinterpret_cast<char*>(lpBatchBuff), cbBatch, 0, nPos);
			if (nPos >= cbTotal)
			{
				bEOS = TRUE;
			}
			
			LEAVE(CryptEncrypt(hKeyAES, hHash, bEOS, 0, lpBatchBuff, &cb, cbBatchBuff));

			BWriteBrick(pNewBrick, lpParam->pShack, lpBatchBuff, cb);
		} while (!bEOS);

		lpParam->cbCrypt = pNewBrick->UntilGetDataSize();


		// Sign Hash

		LEAVE(CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, NULL, &lpParam->cbSig));
		
		lpParam->lpSig = (LPBYTE)malloc(lpParam->cbSig);
		LEAVE(lpParam->lpSig);
		LEAVE(CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, lpParam->lpSig, &lpParam->cbSig));
		

		// Encrypt session key

		lpParam->cbSessionKey = m_pProv->ExportSessionKey(hKeyAES, lpParam->hKeyCrypt, &lpParam->lpSesionKey);
		LEAVE(lpParam->cbSessionKey);


		// Hash All (Signed Hash + Encrypted Session Key + Encrypted Data)

		CryptDestroyHash(hHash);
		LEAVE(CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash));
		
		LEAVE(CryptHashData(hHash, lpParam->lpSig, lpParam->cbSig, 0));

		LEAVE(CryptHashData(hHash, lpParam->lpSesionKey, lpParam->cbSessionKey, 0));

		auto* p = pNewBrick;
		do
		{
			if (!CryptHashData(hHash, (LPBYTE)p->GetMemory(), p->GetDataSize(), 0))
			{
				__leave;
			}

			p = p->GetNext();
		} while (p);

		LEAVE(CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&lpParam->cbHash, &cbDataSize, 0));
		
		lpParam->lpHash = (LPBYTE)malloc(lpParam->cbHash);
		LEAVE(CryptGetHashParam(hHash, HP_HASHVAL, lpParam->lpHash, &lpParam->cbHash, 0));
		

		LEAVE(lpParam->pShack->Replace(pBrick, pNewBrick, lpParam->dwPos1, lpParam->dwPos2));

		bRet = true;
	}
	__finally
	{
		SAFE_DELETE_ARRAY(lpBatchBuff);

		if (hKeyAES)
		{
			CryptDestroyKey(hKeyAES);
			hKeyAES = NULL;
		}

		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}

		if (!bRet)
		{
			SAFE_FREE(lpParam->lpHash);
			SAFE_FREE(lpParam->lpSig);
			SAFE_FREE(lpParam->lpSesionKey);			
		}

		if (!bRet)
		{
			lpParam->pShack->Push(pNewBrick);
		}
	}

	return bRet;
}

/*
	. Verify Hash(Signed Hash + Encrypted Session Key + Encrypted Data)
	. Decrypt session key by private key
	. Hash session key
	. Decrypt data
	. Hash data
	. Verify Signed-Hash
*/
bool BP4S::Decrypt(LPBP4SPARAM lpParam)
{
	assert(lpParam);

	BMemoryBrick* pBrick = lpParam->pBrick;
	BMemoryBrick* pNewBrick = lpParam->pShack->Pop();
	HCRYPTPROV hProv = m_pProv->GetProvider();
	bool bRet = false;
	HCRYPTHASH hHash = NULL;
	size_t nPos = lpParam->dwPos1;
	LPBYTE lpHash = NULL;
	DWORD cbHash = 0;
	DWORD cbDataSize = B_Size_Of_DWORD;
	HCRYPTKEY hKeyAES = NULL;
	LPBYTE lpBatchBuff = NULL;
	DWORD cbBatch = 0;
	DWORD cbBatchBuff = 0;

	__try
	{
		// Verify Hash(Signed Hash + Encrypted Session Key + Encrypted Data)

		LEAVE(CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash));
		
		LEAVE(CryptHashData(hHash, lpParam->lpSig, lpParam->cbSig, 0));

		LEAVE(CryptHashData(hHash, lpParam->lpSesionKey, lpParam->cbSessionKey, 0));

		do
		{
			if (nPos < pBrick->GetDataSize())
			{
				LEAVE(CryptHashData(hHash, (LPBYTE)(pBrick->GetMemory() + nPos), pBrick->GetDataSize(), 0));
			}

			pBrick = pBrick->GetNext();
			nPos = 0;
		} while (pBrick);

		LEAVE(CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&cbHash, &cbDataSize, 0));

		lpHash = (LPBYTE)malloc(cbHash);
		LEAVE(CryptGetHashParam(hHash, HP_HASHVAL, lpHash, &cbHash, 0));

		if (cbHash != lpParam->cbHash || cbHash != RtlCompareMemory(lpHash, lpParam->lpHash, cbHash))
		{
			__leave;
		}


		// Decrypt & Hash session key

		CryptDestroyHash(hHash);
		LEAVE(CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash));

		hKeyAES = m_pProv->ImportEncryptedKey(&lpParam->lpSesionKey, lpParam->cbSessionKey, lpParam->hKeyCrypt, false);
		LEAVE(hKeyAES);

		LEAVE(CryptHashSessionKey(hHash, hKeyAES, 0));


		// Decrypt & Hash data

		cbBatch = m_dwBatch - m_dwBatch % ENCRYPT_BLOCK_LEN;
		if (ENCRYPT_BLOCK_LEN > 1)
		{
			cbBatchBuff = cbBatch + ENCRYPT_BLOCK_LEN;
		}
		else
		{
			cbBatchBuff = cbBatch;
		}

		lpBatchBuff = new BYTE[cbBatchBuff];		

		pBrick = lpParam->pBrick;
		nPos = lpParam->dwPos1;
		DWORD cbTotal = pBrick->UntilGetDataSize();
		DWORD cb = 0;
		BOOL bEOS = FALSE;
		do
		{
			cb = BReadBrickSparselyUnchecked_char(pBrick, reinterpret_cast<char*>(lpBatchBuff),
				cbBatchBuff, lpParam->dwPos1 == lpParam->dwPos2 ? 0 : lpParam->dwPos2, nPos);
			if (nPos >= cbTotal)
			{
				bEOS = TRUE;
			}

			if (!CryptDecrypt(hKeyAES, hHash, bEOS, 0, lpBatchBuff, &cb))
			{
				__leave;
			}

			BWriteBrick(pNewBrick, lpParam->pShack, lpBatchBuff, cb);
		} while (!bEOS);

		lpParam->cbCrypt = pNewBrick->UntilGetDataSize();


		// Verify Signed-Hash

		LEAVE(CryptVerifySignature(hHash, lpParam->lpSig, lpParam->cbSig, lpParam->hKeySign, NULL, 0));


		LEAVE(lpParam->pShack->Replace(pBrick, pNewBrick, lpParam->dwPos1, lpParam->dwPos2));
		
		bRet = true;
	}
	__finally
	{
		SAFE_DELETE_ARRAY(lpBatchBuff);

		if (hKeyAES)
		{
			CryptDestroyKey(hKeyAES);
			hKeyAES = NULL;
		}

		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}

		SAFE_FREE(lpHash);

		if (!bRet)
		{
			lpParam->pShack->Push(pNewBrick);
		}
	}

	return bRet;
}
