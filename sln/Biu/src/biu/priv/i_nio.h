
#ifndef _I_NIO_H_
#define _I_NIO_H_


#include "bndef.h"
#include "biodisp.h"
#include "bsyncprim.h"
#include "bcoll.h"
#include "biodef.h"
#include "bpeer.h"
#include "bpeersvr.h"
#include "bpeerdata.h"
#include "bstdcollutil.h"
#include "bnpar.h"
#include <atomic>


namespace Biu{


	// Forwarding

	class BNetIOExecutionUnit;
	struct BNConnectionCreation;


	// Struct I_NetIOExecutionUnit

	struct I_NetIOExecutionUnit
	{
		I_NetIOExecutionUnit();
		~I_NetIOExecutionUnit();


		BNetIOExecutionUnit* m_pOutter;

		BCriticalSection m_csClients;
		BCriticalSection m_csServers;
		BCriticalSection m_csData;
		BCriticalSection m_csClientData;
		BCriticalSection m_csIdlePassiveClosed;
		BCriticalSection m_csIdleProactiveClosed;
#ifndef _WINDOWS
		BCriticalSection m_csPendingConnect;
#endif

		BCookieMap<BServerPeer*>					m_servers;
		map<unsigned int, BDataPeer*>				m_data;
		BCookieMap<BPeer*>							m_clients;
		map<unsigned int, BClientDataPeer*>			m_clientData;
		list<BPeer*>								m_listPassiveClosed;
		list<BPeer*>								m_listProactiveClosed;
#ifndef _WINDOWS
		list<BPeer*>								m_pengingConnect;
#endif


		//void ExecuteIO(LPOVERLAPPED_ENTRY poe, BOOL bSucc);
		void ExecuteIO(DWORD cb, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped, BOOL bSucc);
#ifdef _WINDOWS
		void ExecuteWorker(BIOContext* pContext);
#else
		bool ExecuteWorker(BIOContext* pContext, shared_ptr<list<BIOContext*> >& listPending);
#endif

		void Reclaim(BIODispatchable* pDisp);


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

		//bool Connect(
		//	B_Sock_Trait trait,
		//	B_Sock_Type sockType,
		//	IBDataChannel* pProtocol,
		//	BIOHandler* pHandler,
		//	sockaddr_in* psa,
		//	sockaddr_in* psaRemote,
		//	unsigned int& nCookie,
		//	long long llTimeoutSeconds = -1,
		//	unsigned int nRef = 0,
		//	long long llVar = 0,
		//	bool bCompatibleInitializationState = false
		//	);

		bool Listen(BNConnectionCreation& ncc, unsigned int& nCookie);
		bool Connect(BNConnectionCreation& ncc, unsigned int& nCookie);
		bool ConnectData(B_Sock_Trait trait, unsigned int nCookie);

		BPeer* AcquirePeerForConnecting(BNConnectionCreation& ncc, unsigned int& nCookie);
		bool Connect(BPeer* peer, BNConnectionCreation& ncc, unsigned int& nCookie);

		void CloseServer(unsigned int nCookie);
		void CloseServerItem(unsigned int nCookie, BPeerRepresentation* pRepresentation);
		void CloseClient(unsigned int nCookie, bool bNow = true);


		void HandlePending();

		bool PostAccept(BPeer* peer, unsigned int dwCount = 1);
		BPeer* AcquirePeer(B_Sock_Type type, bool accept);


		inline unsigned int AddServer(BServerPeer* pServer)
		{
			unsigned int nCookie = 0;
			{
				BScopeLock sl(&m_csServers);
				nCookie = m_servers.Add(pServer);
			}

			return nCookie;
		}

		inline void RemoveServer(unsigned int nCookie)
		{
			BScopeLock sl(&m_csServers);
			m_servers.RemovePointer(nCookie);
		}

		inline unsigned int AddClient(BPeer* peer)
		{
			unsigned int nCookie = 0;
			{
				BScopeLock sl(&m_csClients);
				nCookie = m_clients.Add(peer);
			}

			return nCookie;
		}

		inline void RemoveClient(unsigned int nCookie)
		{
			BScopeLock sl(&m_csClients);
			m_clients.RemovePointer(nCookie);
		}

		inline void AddPassiveClosed(BPeer* peer)
		{
			BSyncPushBack(m_csIdlePassiveClosed, m_listPassiveClosed, peer);
		}

		inline void AddProactiveClosed(BPeer* peer)
		{
			BSyncPushBack(m_csIdleProactiveClosed, m_listProactiveClosed, peer);
		}

#ifndef _WINDOWS
		inline void AddPendingConnect(BPeer* peer)
		{
			BScopeLock sl(&m_csPendingConnect);
			m_pengingConnect.push_back(peer);
		}

		inline void RemovePendingConnect(BPeer* peer)
		{
			BScopeLock sl(&m_csPendingConnect);
			m_pengingConnect.remove(peer);
		}
#endif

		inline bool ContainsClient(unsigned int nCookie)
		{
			bool bRet = false;
			{
				BScopeLock sl(&m_csClients);
				bRet = m_clients.ContainsKey(nCookie);
			}

			return bRet;
		}

		inline bool ContainsServer(unsigned int nCookie)
		{
			bool bRet = false;
			{
				BScopeLock sl(&m_csServers);
				bRet = m_servers.ContainsKey(nCookie);
			}

			return bRet;
		}

		inline BPeer* GetClient(unsigned int nCookie)
		{
			BPeer* peer = NULL;
			{
				BScopeLock sl(&m_csClients);
				m_clients.GetValue(nCookie, peer);
			}

			return peer;
		}

		inline BServerPeer* GetServer(unsigned int nCookie)
		{
			BServerPeer* pServer = NULL;
			{
				BScopeLock sl(&m_csServers);
				m_servers.GetValue(nCookie, pServer);
			}

			return pServer;
		}

		inline BDataPeer* GetData(unsigned int nCookie)
		{
			return BSyncGetPointer(m_csData, m_data, nCookie);
		}

		inline BClientDataPeer* GetClientData(unsigned int nCookie)
		{
			return BSyncGetPointer(m_csClientData, m_clientData, nCookie);
		}
	};


}


#endif // _I_NIO_H_
