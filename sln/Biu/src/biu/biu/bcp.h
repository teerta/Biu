
#ifndef _BCP_H_
#define _BCP_H_


#include "biu.h"

#ifdef _WINDOWS
#include <wincrypt.h>
#endif


namespace Biu {


	// Class BCryptProvider

	class BIU_API BCryptProvider
	{
	public:
		BCryptProvider(LPCWSTR szProvider = MS_ENH_RSA_AES_PROV, DWORD dwProvType = PROV_RSA_AES);
		~BCryptProvider();

		HCRYPTKEY GenerateKey(ALG_ID algID, DWORD dwFlags);
		DWORD ExportKey(HCRYPTKEY hKey, LPBYTE* lppKeyBlob);
		DWORD ExportKeyPair(HCRYPTKEY hKey, HCRYPTKEY hExpKey, LPBYTE* lppKeyBlob);
		DWORD ExportSessionKey(HCRYPTKEY hKey, HCRYPTKEY hExpKey, LPBYTE* lppKeyBlob);
		HCRYPTKEY ImportKey(LPBYTE* lppKeyBlob, DWORD cbKeyBlob, bool bDeleteBlob = true);
		HCRYPTKEY ImportEncryptedKey(LPBYTE* lppKeyBlob, DWORD cbKeyBlob, HCRYPTKEY hPubKey, bool bDeleteBlob = true);
		HCRYPTKEY DeriveKey(LPBYTE lpKeyBlob, DWORD cbKeyBlob, ALG_ID algHash, ALG_ID algKey, DWORD dwFlag);

		DWORD GetHashResultLength(ALG_ID algID);
		DWORD GetHashSignatureLength(ALG_ID algID);
		DWORD GetKeyLength(ALG_ID algID, DWORD dwFlags, HCRYPTKEY hExpKey);


		inline HCRYPTPROV GetProvider() { return m_hProv; }

		inline HCRYPTKEY GeneratePrivatePublicKeyPair(DWORD dwFlags = CRYPT_EXPORTABLE)
		{
			return GenerateKey(CALG_RSA_SIGN | CALG_RSA_KEYX, dwFlags);
		}

	protected:
		HCRYPTPROV		m_hProv;
	};


}


#endif // _BCP_H_
