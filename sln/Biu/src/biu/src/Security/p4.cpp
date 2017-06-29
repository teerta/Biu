
#include "stdafx.h"
#include "bp4.h"
#include "bcp.h"
#include "bbase.h"


using namespace Biu;


#define B_RSA_1024_LEN					128
#define B_RSA_2048_LEN					256
#define B_PKCS_1_PADDING_MIN_LEN		11


// BP4

BP4::BP4(DWORD dwBatch)
	: m_dwBatch(dwBatch)
	, m_dwHashLen(0)
	, m_dwSigLen(0)
{

}

BP4::~BP4()
{

}

bool BP4::HashData(LPBYTE pData, DWORD cbData, LPBYTE* ppResult, DWORD* pcbResult, bool bSign, HCRYPTKEY hKeySign)
{
	assert(pData);
	assert(cbData);
	assert(ppResult);
	assert(pcbResult);
	assert(hKeySign);

	HCRYPTPROV hProv = m_pProv->GetProvider();
	bool bRet = false;
	HCRYPTHASH hHash = NULL;
	DWORD cbTotal = 0;
	DWORD cb = 0;
	*ppResult = NULL;

	__try
	{
		if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
		{
			__leave;
		}

		while (cbTotal != cbData)
		{
			cb = min<DWORD>(cbData, m_dwBatch);
			if (!CryptHashData(hHash, pData + cbTotal, cb, 0))
			{
				__leave;
			}

			cbTotal += cb;
			cbData -= cb;
		}

		if (bSign)
		{
			if (!CryptSignHash(hHash, hKeySign, NULL, 0, NULL, pcbResult))
			{
				__leave;
			}

			*ppResult = (LPBYTE)malloc(*pcbResult);
			if (!CryptSignHash(hHash, hKeySign, NULL, 0, *ppResult, pcbResult))
			{
				__leave;
			}
		}
		else
		{
			if (!CryptGetHashParam(hHash, HP_HASHSIZE, NULL, pcbResult, 0))
			{
				__leave;
			}

			*ppResult = (LPBYTE)malloc(*pcbResult);
			if (!CryptGetHashParam(hHash, HP_HASHVAL, *ppResult, pcbResult, 0))
			{
				__leave;
			}
		}


		bRet = true;
	}
	__finally
	{
		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}

		if (!bRet && *ppResult)
		{
			free(*ppResult);
			*ppResult = NULL;
		}
	}

	return bRet;
}

bool BP4::Encrypt(LPBP4PARAM lpParam)
{
	assert(lpParam);

	BMemoryBrick* pBrick = lpParam->pBrick;
	BMemoryBrick* pNewBrick = lpParam->pShack->Pop();
	HCRYPTPROV hProv = m_pProv->GetProvider();
	bool bRet = false;
	HCRYPTHASH hHash = NULL;
	size_t nPos = lpParam->dwPos1;
	DWORD cbDataSize = B_Size_Of_DWORD;
	BYTE buff[B_RSA_1024_LEN];
	
	__try
	{
		// Hash Data, Sign the Hash & Encrypt Data

		if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
		{
			__leave;
		}

		DWORD cbTotal = pBrick->UntilGetDataSize();
		DWORD cb = 0;
		BOOL bEOS = FALSE;
		do
		{
			cb = BReadBrickSparselyUnchecked_char(pBrick, reinterpret_cast<char*>(buff), B_RSA_1024_LEN - B_PKCS_1_PADDING_MIN_LEN, 0, nPos);
			if (nPos >= cbTotal)
			{
				bEOS = TRUE;
			}
			;
			if (!CryptEncrypt(lpParam->hKeyCrypt, hHash, bEOS, 0, buff, &cb, B_RSA_1024_LEN))
			{
				__leave;
			}

			BWriteBrick(pNewBrick, lpParam->pShack, buff, cb);
		} while (!bEOS);

		lpParam->cbCrypt = pNewBrick->UntilGetDataSize();
		
		if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, NULL, &lpParam->cbSig))
		{
			__leave;
		}

		lpParam->lpSig = (LPBYTE)malloc(lpParam->cbSig);
		LEAVE(lpParam->lpSig);
		if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, lpParam->lpSig, &lpParam->cbSig))
		{
			__leave;
		}

		// Hash Signed-Hash + Encrypted-Data

		CryptDestroyHash(hHash);
		if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
		{
			__leave;
		}

		if (!CryptHashData(hHash, lpParam->lpSig, lpParam->cbSig, 0))
		{
			__leave;
		}

		auto* p = pNewBrick;
		do
		{
			if (!CryptHashData(hHash, (LPBYTE)p->GetMemory(), p->GetDataSize(), 0))
			{
				__leave;
			}

			p = p->GetNext();
		} while (p);		

		if (!CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&lpParam->cbHash, &cbDataSize, 0))
		{
			__leave;
		}

		lpParam->lpHash = (LPBYTE)malloc(lpParam->cbHash);
		if (!CryptGetHashParam(hHash, HP_HASHVAL, lpParam->lpHash, &lpParam->cbHash, 0))
		{
			__leave;
		}

		if (!lpParam->pShack->Replace(pBrick, pNewBrick, lpParam->dwPos1, lpParam->dwPos2))
		{
			__leave;
		}

		bRet = true;
	}
	__finally
	{
		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}

		if (!bRet)
		{
			if (lpParam->lpHash)
			{
				free(lpParam->lpHash);
				lpParam->lpHash = NULL;
			}

			if (lpParam->lpSig)
			{
				free(lpParam->lpSig);
				lpParam->lpSig = NULL;
			}
		}

		if (!bRet)
		{
			lpParam->pShack->Push(pNewBrick);
		}
	}

	return bRet;
}

bool BP4::Decrypt(LPBP4PARAM lpParam)
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
	LPBYTE lpSig = NULL;
	DWORD cbSig = 0;
	DWORD cbDataSize = B_Size_Of_DWORD;
	BYTE buff[B_RSA_1024_LEN];

	__try
	{
		if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
		{
			__leave;
		}

		if (!CryptHashData(hHash, lpParam->lpSig, lpParam->cbSig, 0))
		{
			__leave;
		}

		do
		{
			if (nPos < pBrick->GetDataSize())
			{
				if (!CryptHashData(hHash, (LPBYTE)(pBrick->GetMemory() + nPos), pBrick->GetDataSize(), 0))
				{
					__leave;
				}
			}			

			pBrick = pBrick->GetNext();
			nPos = 0;
		} while (pBrick);		

		if (!CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&cbHash, &cbDataSize, 0))
		{
			__leave;
		}

		lpHash = (LPBYTE)malloc(cbHash);
		LEAVE(lpHash);
		if (!CryptGetHashParam(hHash, HP_HASHVAL, lpHash, &cbHash, 0))
		{
			__leave;
		}

		if (cbHash != lpParam->cbHash || cbHash != RtlCompareMemory(lpHash, lpParam->lpHash, cbHash))
		{
			__leave;
		}


		CryptDestroyHash(hHash);
		if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
		{
			__leave;
		}

		pBrick = lpParam->pBrick;
		nPos = lpParam->dwPos1;
		DWORD cbTotal = pBrick->UntilGetDataSize();
		DWORD cb = 0;
		BOOL bEOS = FALSE;
		do
		{
			cb = BReadBrickSparselyUnchecked_char(pBrick, reinterpret_cast<char*>(buff), 
				B_RSA_1024_LEN, lpParam->dwPos1 == lpParam->dwPos2 ? 0 : lpParam->dwPos2, nPos);
			if (nPos >= cbTotal)
			{
				bEOS = TRUE;
			}

			if (!CryptDecrypt(lpParam->hKeyCrypt, hHash, bEOS, 0, buff, &cb))
			{
				__leave;
			}

			BWriteBrick(pNewBrick, lpParam->pShack, buff, cb);
		} while (!bEOS);

		lpParam->cbCrypt = pNewBrick->UntilGetDataSize();

		if (!CryptVerifySignature(hHash, lpParam->lpSig, lpParam->cbSig, lpParam->hKeySign, NULL, 0))
		{
			__leave;
		}

		if (!lpParam->pShack->Replace(pBrick, pNewBrick, lpParam->dwPos1, lpParam->dwPos2))
		{
			__leave;
		}

		bRet = true;
	}
	__finally
	{
		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}

		if (lpHash)
		{
			free(lpHash);
			lpHash = NULL;
		}

		if (lpSig)
		{
			free(lpSig);
			lpSig = NULL;
		}

		if (!bRet)
		{
			lpParam->pShack->Push(pNewBrick);
		}
	}

	return bRet;
}
