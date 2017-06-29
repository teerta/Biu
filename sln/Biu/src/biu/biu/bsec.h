
#ifndef _BSEC_H_
#define _BSEC_H_


#include "biu.h"
#include "bsyncprim.h"

#ifdef _WINDOWS
#include <wincrypt.h>
#endif


namespace Biu {


	// Forwarding

	class BCryptProvider;
	class BP4;
	class BP4S;


	// Class BSecurityItem

	class BIU_API BSecurityItem
	{
	public:
		
		enum Alg
		{
			Alg_P4			= 0x00000001,
			Alg_P4S			= 0x00000002
		};
		

		BSecurityItem(Alg alg, ALG_ID algID, DWORD dwFlags);
		~BSecurityItem();

		BSecurityItem(const BSecurityItem&) = delete;
		BSecurityItem& operator=(const BSecurityItem&) = delete;


		BSecurityItem* GetLastItem();


		inline long AddRef()
		{
			return InterlockedIncrement(&m_lRef);
		}

		inline long Release()
		{
			return InterlockedDecrement(&m_lRef);
		}

		inline long GetRefCount()
		{
			return InterlockedCompareExchange(&m_lRef, 0, 0);
		}

		inline BCryptProvider* GetCryptProvider() { return m_pProv; }
		
		inline void SetNext(BSecurityItem* p) { m_pNext = p; }
		inline BSecurityItem* GetNext() { return m_pNext; }

		void SetKey(HCRYPTKEY hKey) { m_hKey = hKey; }
		inline HCRYPTKEY GetKey() { return m_hKey; }

		BP4* GetAlgP4() { return m_pAlgP4; }
		BP4S* GetAlgP4S() { return m_pAlgP4S; }

	private:
		Alg	m_alg;
		BSecurityItem* m_pNext;
		BCryptProvider* m_pProv;
		BP4* m_pAlgP4;
		BP4S* m_pAlgP4S;
		volatile long m_lRef;
		HCRYPTKEY m_hKey;
	};


	// Class BSecurityProvider

	class BIU_API BSecurityProvider
	{
	public:
		BSecurityProvider();
		~BSecurityProvider();

		BSecurityProvider(const BSecurityProvider&) = delete;
		BSecurityProvider& operator=(const BSecurityProvider&) = delete;

		BSecurityItem* GetItem(BSecurityItem* pLastItem);
		BSecurityItem* NewItem(BSecurityItem::Alg alg = BSecurityItem::Alg_P4S);

	private:
		BSecurityItem* m_pItem;		
		volatile long m_lItem;

		BCriticalSection m_csItem;

		void EnsureItem(BSecurityItem* pItem);
		void DeleteItem(BSecurityItem* pItem);
	};


}


#endif // _BSEC_H_
