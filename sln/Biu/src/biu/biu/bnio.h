
#ifndef _BNIO_H_
#define _BNIO_H_


#include "biu.h"
#include "bndef.h"
#include "biodisp.h"
#include "bpeer.h"
#include "bpeersvr.h"
#include "bpeerdata.h"
#include "bnpar.h"


namespace Biu {


	// Forwarding

	class BPeer;
	class BServerPeer;


	// Class BNetIOExecutionUnit

	class BIU_API BNetIOExecutionUnit : public BIOExecutionUnit
	{
	public:
		BNetIOExecutionUnit();
		BNetIOExecutionUnit(const BNetIOExecutionUnit& rhs) = delete;
		virtual ~BNetIOExecutionUnit();
		
		BNetIOExecutionUnit& operator=(const BNetIOExecutionUnit& rhs) = delete;

		//virtual void ExecuteIO(LPOVERLAPPED_ENTRY poe, BOOL bSucc);
		virtual void ExecuteIO(DWORD cb, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped, BOOL bSucc);
#ifdef _WINDOWS
		virtual void ExecuteWorker(BIOContext* pContext);
#else
		virtual bool ExecuteWorker(BIOContext* pContext, shared_ptr<list<BIOContext*> >& listPending);
#endif

		virtual void Reclaim(BIODispatchable* pDisp);


//#ifdef _WINDOWS
//		bool Listen(
//			B_Sock_Trait trait, 
//			B_Sock_Type sockType,
//			IBDataChannel* pProtocol,
//			BIOHandler* pHandler,
//			sockaddr_in* psa,
//			unsigned int& nCookie,
//			unsigned int nAccepts = 10,
//			long long llTimeoutSeconds = -1,
//			int nBacklog = 64,
//			long long llVar = 0,
//			bool bCompatibleInitializationState = false
//			);
//#else
//		bool Listen(
//			B_Sock_Type sockType,
//			sockaddr_in* psa,
//			unsigned int& nCookie,
//			long long llTimeoutSeconds = -1,
//			int nBacklog = 64,
//			long long llVar = 0,
//			bool bCompatibleInitializationState = false
//			);
//#endif
//
//		bool Connect(
//			B_Sock_Trait trait,
//			B_Sock_Type sockType,
//			IBDataChannel* pProtocol,
//			BIOHandler* pHandler,
//			sockaddr_in* psa,
//			sockaddr_in* psaRemote,
//			unsigned int& nCookie,
//			long long llTimeoutSeconds = -1,
//			unsigned int nRef = 0,
//			long long llVar = 0,
//			bool bCompatibleInitializationState = false
//			);

		bool Listen(BNConnectionCreation& ncc, unsigned int& nCookie);
		bool Connect(BNConnectionCreation& ncc, unsigned int& nCookie);
		bool ConnectData(B_Sock_Trait trait, unsigned int nCookie);

		BPeer* AcquirePeerForConnecting(BNConnectionCreation& ncc, unsigned int& nCookie);
		bool Connect(BPeer* peer, BNConnectionCreation& ncc, unsigned int& nCookie);

		void CloseServer(unsigned int nCookie);
		void CloseServerItem(unsigned int nCookie, BPeerRepresentation* pRepresentation);
		void CloseClient(unsigned int nCookie, bool bNow = true);

		bool ContainsClient(unsigned int nCookie);
		bool ContainsServer(unsigned int nCookie);

		BPeer* GetClient(unsigned int nCookie);
		BServerPeer* GetServer(unsigned int nCookie);
		
		BDataPeer* GetData(unsigned int nCookie);
		BClientDataPeer* GetClientData(unsigned int nCookie);


		inline BHANDLE GetIntern() { return m_pIntern; }

	protected:
		BHANDLE				m_pIntern;		
	};


}


#endif // _BNIO_H_
