
#ifndef _BIOSECC_H_
#define _BIOSECC_H_


#include "biu.h"
#include "btype.h"
#include "bsec.h"
#include "bcp.h"

#ifdef _WINDOWS
#include <wincrypt.h>
#endif


namespace Biu {


	// Forwarding

	class BSecurityProvider;
	class BSecurityItem;


	class BIU_API BIOSecurityContext
	{
	public:
		BIOSecurityContext(BSecurityProvider* pProv);
		virtual ~BIOSecurityContext();

		BIOSecurityContext(const BIOSecurityContext&) = delete;
		BIOSecurityContext& operator=(const BIOSecurityContext&) = delete;

		bool ChangeRemoteKey(LPBYTE lpKey, DWORD cbKey);
		bool ChangeRemoteKeyStatic(LPBYTE lpKey, DWORD cbKey);
		BSecurityItem* GetSecurityItem();


		BSecurityProvider* GetSecurity() { return m_pProv; }
		BRawData* GetRemoteKeyRaw() { return &m_remoteKeyRaw; }
		HCRYPTKEY GetRemoteKey() { return m_hKeyRemote; }

		inline BSecurityItem* GetSecurityItemStatic() { return m_pLastItem; }
		
		inline DWORD ExportKeyStatic(LPBYTE* lppKeyBlob) 
		{ 
			return m_pLastItem->GetCryptProvider()->ExportKey(m_pLastItem->GetKey(), lppKeyBlob); 
		}

		inline bool HasRemoteKeyStatic() { return m_remoteKeyRaw.HasData(); }


	protected:
		BSecurityProvider* m_pProv;
		BSecurityItem* m_pLastItem;
		HCRYPTKEY m_hKeyRemote;
		BRawData m_remoteKeyRaw;

		bool Create();
	};


}


#endif // _BIOSECC_H_
