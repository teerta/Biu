
#ifndef _BPEERDATA_H_
#define _BPEERDATA_H_


#include "biu.h"
#include "bpeer.h"
#include "bpeersvr.h"
#include "bstdcollutil.h"
#include <random>

#ifndef _WINDOWS
#include <atomic>
#endif


using namespace std;


namespace Biu {


	/*
		Transmit a file
		
		1. [Sending-Side, Main-Peer] Call BDataPeer's {Send}
		2. [Sending-Side, Main-Peer] Send notification of sending to Receiving-Side
		3. [Receving-Side, Main-Peer] Receive notification of sending
		4. [Receving-Side, Main-Peer] Send back Ack of receiving (Yes or No)
		5. [Sending-Side, Main-Peer] Receive Ack of receiving, and cancel sending if Ack-No

		6. [Sending-Side, Main-Peer] Call BDataPeer's {Transmit}

	*/


	#define BDT_ID(v)		((v) & 0x7fffffff)
	#define BDT_RECVID(v)	((v) | 0x80000000)


	// Forwarding

	class BDataPeer;
	class BClientDataPeer;


	// Struct BDataContinuation

	struct BDataContinuation
	{
		// For send: SendFileID; For recv: RecvFileID. U need BDT_ID and BDT_RECVID here.
		int m_nID;

		// true for send and false for recv
		bool m_bDir;
		unsigned long long m_cb;
		unsigned long long m_ullOrigin;
		unsigned long long m_ullFinished;
		BPeer* m_primaryPeer;
		CString m_strFile;
	};


	// Class BDataInfo

	class BIU_API BDataInfo
	{
		friend class BDataPeer;
		friend class BClientDataPeer;

	public:
		BDataInfo();
		virtual ~BDataInfo();

		bool PrepareForRecv(int nID, unsigned long long cb, LPCTSTR lpszFile);
		bool PrepareForSend(int nID, LPCTSTR lpszFile);
		void CloseFile();

		void GetFile(CString& str);

		unsigned long long Read(void* pBuff, unsigned long long cb, bool bUpdateFinished = true, 
			bool bNewOrigin = false, unsigned long long ullNewPos = 0);

		unsigned long long Write(void* pBuff, unsigned long long cb, bool bUpdateFinished = true, 
			bool bNewOrigin = false, unsigned long long ullNewPos = 0);

		void CreateContinuation(BDataContinuation& continuation);

		inline int GetID() { return m_nID; }
		inline unsigned long long GetSize() { return m_cb; }
		inline HANDLE GetHandle() { return m_hFile; }
		inline bool GetDirection() { return m_bDir; }

		inline void SetTransmitting(bool bTransmitting = true) 
		{ 
#ifdef _WINDOWS
			InterlockedExchange(&m_bTransmitting, (LONG)bTransmitting);
#else
			m_bTransmitting = b;
#endif
		}

		inline bool IsTransmitting() 
		{ 
#ifdef _WINDOWS
			return (bool)InterlockedCompareExchange(&m_bTransmitting, 0L, 0L);
#else
			return m_bTransmitting;
#endif
		}		

		inline void SetPeer(BPeer* peer) { m_peer = peer; }
		inline BPeer* GetPeer() { return m_peer; }

		inline void SetPrimaryPeer(BPeer* peer) { m_primaryPeer = peer; }
		inline BPeer* GetPrimaryPeer() { return m_primaryPeer; }

		inline void SetOrigin(unsigned long long ull) { m_ullOrigin = ull; }
		inline unsigned long long GetOrigin() { return m_ullOrigin; }

		inline void IncFinished(unsigned long long ull) { m_ullFinished += ull; }
		inline void DecFinished(unsigned long long ull) { m_ullFinished -= ull; }
		inline unsigned long long GetFinished() { return m_ullFinished; }
		inline bool IsFinished() { return m_ullFinished == m_cb; }

		inline unsigned long long GetAvailable() { return m_cb - m_ullFinished; }

		inline void SetHeader(void* p) { m_pHeader = p; }
		inline void* GetHeader() { return m_pHeader; }

		inline void SetParameter(void* p) { m_pParam = p; }
		inline void* GetParameter() { return m_pParam; }

	private:
		bool						m_bDir;
		CString						m_strFile;
		int							m_nID;
		unsigned long long			m_cb;
		unsigned long long			m_ullOrigin;
		unsigned long long			m_ullFinished;
		HANDLE						m_hFile;
		HANDLE						m_hFileMapping;
		void*						m_lpv;
		BPeer*						m_peer;
		BPeer*						m_primaryPeer;
		void*						m_pHeader;
		void*						m_pParam;

#ifdef _WINDOWS
		volatile LONG				m_bTransmitting;
#else
		atomic_bool					m_bTransmitting;
#endif
		

		bool MapFile();
	};


	// Class BWaitDataInfo

	class BIU_API BWaitDataInfo
	{
	public:
		BWaitDataInfo();
		virtual ~BWaitDataInfo();

		inline void SetFile(LPCTSTR lpszFile) { m_strFile = lpszFile; }
		inline const CString& GetFile() const { return m_strFile; }

		inline void SetDirection(bool bOut) { m_bDir = bOut; }
		inline bool GetDirection() { return m_bDir; }

	private:
		CString				m_strFile;
		bool				m_bDir;
	};


	// Class BPeerDataInfo

	class BIU_API BPeerDataInfo
	{
		friend class BDataPeer;
		friend class BClientDataPeer;

	public:
		BPeerDataInfo();
		virtual ~BPeerDataInfo();

	private:
		BCriticalSection			m_csPending;
		BCriticalSection			m_csPeer;
		BCriticalSection			m_csTransmitting;
		BCriticalSection			m_csWait;
		
		map<int, BDataInfo*>		m_mapTransmitting;
		map<int, BDataInfo*>		m_mapPending;
		list<BPeer*>				m_listPeer;
		list<BWaitDataInfo*>		m_listWait;

		bool						m_bTerminate;


		void AddPending(BDataInfo* pDI);
		BDataInfo* GetPending(int nID, bool bRemove = true);

		void PushPeer(BPeer* peer);
		BPeer* PopPeer();
		
		void AddTransmitting(BDataInfo* pDI);
		BDataInfo* GetTransmitting(int nID, bool bRemove = true);
		size_t GetTransmittingSize();
		
		void PushWait(LPCTSTR lpszFile, bool bDir);
		BWaitDataInfo* PopWait();
		size_t GetWaitSize();

		void ClearWorking();


		inline void SetTerminate(bool b) { m_bTerminate = b; }
	};


	// Class BDataPeer

	class BIU_API BDataPeer : public BServerPeer
	{
	public:
		BDataPeer(BPeer* peer);
		BDataPeer(const BDataPeer& rhs) = delete;
		virtual ~BDataPeer();

		BDataPeer& operator=(const BDataPeer& rhs) = delete;

		// [Main Peer] Init a Sending
		/*
			Return: true & pDI is NULL - Over max-send, wait
					true & pDI is not null - success
					false - failed
		*/
		bool Send(BPeer* primaryPeer, LPCTSTR lpszFile, BDataInfo*& pDI);

		// [Main Peer] Init a Receiving
		//BDataInfo* Recv(BPeer* primaryPeer, LPCTSTR lpszDir, LPCTSTR lpszName = NULL);
		bool Recv(BPeer* primaryPeer, int nID, unsigned long long cb, LPCTSTR lpszFile, BDataInfo*& pDI);

		// [Main Peer] After Ack's Confirmation
		/*
			Return: true - peer is the peer that should send (Data Peer). SEND NOW
						{Transmit} is not necessary here

					false - there is no free Data Peer. PENDING
						Call {Transmit} when CONNECTED ot ACCEPTED
		*/
		bool PreTransmit(int nID, BPeer*& peer, BDataInfo** ppDI = NULL);

		// [Data Peer] when CONNECTED ot ACCEPTED
		/*
			Do transmitting when return value is not null.
		*/
		BDataInfo* Transmit(int nID, BPeer* peer);

		BDataInfo* TransmitDirectly(int nID, BPeer* peer);

		// [Data Peer] Call thie function when transmitting is done
		/*
			Return: true - there is more wait Transmtting. SEND AGAIN
						primaryPeer is the [Main Peer]
						strNextWaitFile is the next-file-to-send

					false - no more Transmitting
		*/
		bool PostTransmit(int nID, BPeer*& primaryPeer, BWaitDataInfo*& pWait);

		// [Everywhere] Cancel all transmittings on a Main Peer (Close all data connections)
		void TerminateTransmit(BPeer* primaryPeer, BPeerDataInfo** ppPDI = NULL);

		// [Everywhere] Cancel a transmitting
		void TerminateTransmit(int nID);


		BDataInfo* GetDataInfo(int nID);

		void PurgePeerTransmit(BPeerDataInfo* pPDI);


		inline void SetMaxSend(int n = -1)
		{
#ifdef _WINDOWS
			InterlockedExchange(&m_nMaxSend, (LONG)n);
#else
			m_nMaxSend = b;
#endif
		}

		inline int GetMaxSend()
		{
#ifdef _WINDOWS
			return (bool)InterlockedCompareExchange(&m_nMaxSend, 0L, 0L);
#else
			return m_nMaxSend;
#endif
		}

		inline void SetMaxRecv(int n = -1)
		{
#ifdef _WINDOWS
			InterlockedExchange(&m_nMaxRecv, (LONG)n);
#else
			m_nMaxRecv = b;
#endif
		}

		inline int GetMaxRecv()
		{
#ifdef _WINDOWS
			return (bool)InterlockedCompareExchange(&m_nMaxRecv, 0L, 0L);
#else
			return m_nMaxRecv;
#endif
		}

	private:
#ifdef _WINDOWS
		volatile LONG							m_nMaxSend;
		volatile LONG							m_nMaxRecv;
#else
		atomic_int								m_nMaxSend;
		atomic_int								m_nMaxRecv;
#endif

		uniform_int_distribution<int>			m_keyRandGen;
		mt19937									m_keyRandRng;
		
		BCriticalSection						m_csData;
		BCriticalSection						m_csPeerData;
		
		map<int, BDataInfo*>					m_mapData;
		map<BPeer*, BPeerDataInfo*>				m_mapPeerData;


		int NewID();
	};


	// Class BClientDataPeer

	class BIU_API BClientDataPeer
	{
	public:
		BClientDataPeer(BPeer* peer);
		BClientDataPeer(const BClientDataPeer& rhs) = delete;
		virtual ~BClientDataPeer();

		BClientDataPeer& operator=(const BClientDataPeer& rhs) = delete;

		bool Send(LPCTSTR lpszFile, BDataInfo*& pDI);
		bool Recv(int nID, unsigned long long cb, LPCTSTR lpszFile, BDataInfo*& pDI);
		bool PreTransmit(int nID, BPeer*& peer, BDataInfo** ppDI = NULL);
		BDataInfo* Transmit(int nID, BPeer* peer);
		bool PostTransmit(int nID, BPeer*& primaryPeer, BWaitDataInfo*& pWait);
		void TerminateTransmit(int nID);

		BDataInfo* GetDataInfo(int nID);


		inline void SetMaxSend(int n = -1) 
		{ 
#ifdef _WINDOWS
			InterlockedExchange(&m_nMaxSend, (LONG)n);
#else
			m_nMaxSend = b;
#endif
		}

		inline int GetMaxSend() 
		{ 
#ifdef _WINDOWS
			return (bool)InterlockedCompareExchange(&m_nMaxSend, 0L, 0L);
#else
			return m_nMaxSend;
#endif
		}

		inline void SetMaxRecv(int n = -1) 
		{ 
#ifdef _WINDOWS
			InterlockedExchange(&m_nMaxRecv, (LONG)n);
#else
			m_nMaxRecv = b;
#endif
		}

		inline int GetMaxRecv() 
		{ 
#ifdef _WINDOWS
			return (bool)InterlockedCompareExchange(&m_nMaxRecv, 0L, 0L);
#else
			return m_nMaxRecv;
#endif
		}		

	private:
#ifdef _WINDOWS
		volatile LONG							m_nMaxSend;
		volatile LONG							m_nMaxRecv;
#else
		atomic_int								m_nMaxSend;
		atomic_int								m_nMaxRecv;
#endif

		uniform_int_distribution<int>			m_keyRandGen;
		mt19937									m_keyRandRng;
		
		BPeerDataInfo*							m_pPDI;
		BPeer*									m_primaryPeer;

		BCriticalSection						m_csData;

		map<int, BDataInfo*>					m_mapData;


		int NewID();
	};


}


#endif // _BPEERDATA_H_
