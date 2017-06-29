
#ifndef _BP4S_H_
#define _BP4S_H_


#include "biu.h"
#include "bmshack.h"


namespace Biu {


	// Forwarding

	class BCryptProvider;


	// Struct BP4SParam

	typedef struct tagBP4SPARAM
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
		LPBYTE lpSesionKey;
		DWORD cbSessionKey;
		DWORD cbCrypt;
	} BP4SPARAM, *LPBP4SPARAM;


	// Class BP4S

	class BIU_API BP4S
	{
	public:		
		BP4S(DWORD dwBatch = 8192);
		~BP4S();

		bool Encrypt(LPBP4SPARAM lpParam);
		bool Decrypt(LPBP4SPARAM lpParam);


		inline void SetProvider(BCryptProvider* pProv) { m_pProv = pProv; }

		inline DWORD GetHashLen() { return m_dwHashLen; }
		inline void SetHashLen(DWORD val) { m_dwHashLen = val; }

		inline DWORD GetSigLen() { return m_dwSigLen; }
		inline void SetSigLen(DWORD val) { m_dwSigLen = val; }

		inline DWORD GetKeyLen() { return m_dwKeyLen; }
		inline void SetKeyLen(DWORD val) { m_dwKeyLen = val; }


	private:
		BCryptProvider* m_pProv;
		DWORD m_dwBatch;
		DWORD m_dwHashLen;
		DWORD m_dwSigLen;
		DWORD m_dwKeyLen;
	};


}


#endif // _BP4S_H_
