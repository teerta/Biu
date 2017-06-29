
#ifndef _BPEERSVR_H_
#define _BPEERSVR_H_


#include "biu.h"
#include "bpeer.h"
#include "biorule.h"

#ifndef _WINDOWS
#include <atomic>
#endif


namespace Biu {


	// Forwarding

	class BDataPeer;


	// Class BServerPeer

	class BIU_API BServerPeer
	{
	public:		
		BServerPeer();
		BServerPeer(BPeer* peer);
		BServerPeer(BServerPeer& rhs);
		virtual ~BServerPeer();

		BServerPeer& operator=(BServerPeer& rhs);
		BPeer* operator[](BPeerRepresentation* pRepresentation);

		void AddPeer(BPeer* peer);
		void RemovePeer(BPeer* peer);
		BPeer* RemovePeer(BPeerRepresentation* pRepresentation);
		BPeer* GetPeer(BPeerRepresentation* pRepresentation);
		BPeer* GetPeer(unsigned int nID);

		void AddPendingAccept(BPeer* peer);
		void RemovePendingAccept(BPeer* peer);
		void FinishPending(BPeer* peer, bool bSucc = true);

		inline BPeer* GetServerPeer() const { return m_pSP; }		

		inline B_IO_Read_Mode GetMode() { return m_mode; }
		inline void SetMode(B_IO_Read_Mode mode) { m_mode = mode; }

		inline void SetRunning(bool b = true) 
		{
#ifdef _WINDOWS
			InterlockedExchange(&m_bRunning, (LONG)b);
#else
			m_bRunning = b; 
#endif
		}

		inline bool IsRunning() 
		{
#ifdef _WINDOWS
			return (bool)InterlockedCompareExchange(&m_bRunning, 0L, 0L);
#else
			return m_bRunning; 
#endif
		}

		inline void SetTerminate(bool b) { m_bTerminate = b; }
		
		inline BIOBehaviorArbiter* GetBehaviorArbiter() { return m_pBehaviorArbiter; }

	protected:
		BCriticalSection m_cs;
		BCriticalSection m_csPendingAccept;

		BPeer* m_pSP;				
		B_IO_Read_Mode m_mode;
		bool m_bTerminate;

#ifdef _WINDOWS		
		volatile LONG m_bRunning;
#else
		atomic_bool m_bRunning;
#endif

		BIOBehaviorArbiter* m_pBehaviorArbiter;


		map<BPeerRepresentation*, BPeer*>			m_peers;
		map<BPeerRepresentation*, BPeer*>			m_pendingAccept;


		void Copy(BServerPeer& rhs);

		void Close();
		void CloseServer();
		void CloseClients();
	};


}


#endif // _BPEERSVR_H_
