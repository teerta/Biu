
#ifndef _BP4_H_
#define _BP4_H_

// Private/Public key Pair (non-Public)


#include "biu.h"
#include "bmshack.h"


namespace Biu {


	// Forwarding

	class BCryptProvider;


	// Struct BP4Param

	typedef struct tagBP4PARAM
	{
		BMemoryShack_char* pShack;
		BMemoryBrick* pBrick;
		HCRYPTKEY hKeySign;
		HCRYPTKEY hKeyCrypt;
		size_t dwPos1;
		size_t dwPos2;
		LPBYTE lpSig;
		DWORD cbSig;
		LPBYTE lpHash;
		DWORD cbHash;		
		DWORD cbCrypt;
	} BP4PARAM, *LPBP4PARAM;


	// Class BP4

	class BIU_API BP4
	{
	public:
		BP4(DWORD dwBatch = 8192);
		~BP4();

		//bool Encrypt(LPBYTE pData, DWORD cbData, HCRYPTKEY hKeySign, HCRYPTKEY hKeyEncrypt, LPBYTE* ppResult, DWORD* pcbResult);
		bool Encrypt(LPBP4PARAM lpParam);
		bool Decrypt(LPBP4PARAM lpParam);


		inline void SetProvider(BCryptProvider* pProv) { m_pProv = pProv; }

		inline DWORD GetHashLen() { return m_dwHashLen; }
		inline void SetHashLen(DWORD val) { m_dwHashLen = val; }

		inline DWORD GetSigLen() { return m_dwSigLen; }
		inline void SetSigLen(DWORD val) { m_dwSigLen = val; }


	private:
		BCryptProvider* m_pProv;
		DWORD m_dwBatch;
		DWORD m_dwHashLen;
		DWORD m_dwSigLen;

		bool HashData(LPBYTE pData, DWORD cbData, LPBYTE* ppResult, DWORD* pcbResult, bool bSign, HCRYPTKEY hKeySign = NULL);
	};


}


#endif // _BP4_H_
