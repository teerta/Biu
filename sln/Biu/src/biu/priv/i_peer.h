
#ifndef _I_PEER_H_
#define _I_PEER_H_


#include "bndef.h"
#include "priv/i_iodispab.h"
#include "bpeerrepre.h"
#include "bpeerdata.h"
#include <atomic>


namespace Biu {


	// Forwarding

	class BPeer;
	class BPeerDataInfo;


	// Struct I_Peer

	struct I_Peer : public I_IODispatchable
	{
		I_Peer();
		virtual ~I_Peer();


		BPeerRepresentation m_representation;
		BPeerRepresentation m_remoteRepresentation;

#ifdef _WINDOWS
		SOCKET m_s;
#else
		int m_s;
#endif
		bool m_bServer;
		unsigned int m_nID;
		unsigned int m_nServerID;
		unsigned int m_nRefID;
		B_Sock_Type m_sockType;
		int m_nSendFlags;
		int m_nRecvFlags;
		BServerPeer* m_pServer;
		B_Sock_Trait m_trait;
		void* m_traitVal;

		atomic<B_Peer_Close_Flag> m_closeFlag;
		atomic_bool m_bCompatibleInitialization;

		//BCriticalSection m_csReadShack;

#ifdef _WINDOWS
		BSockExMethod* m_pEx;
#endif


		/*
		* Send a value to peer (Other threads)
		*/
		B_Peer_Transmit_Result Send(IBSerializable* pObj, void* p);
		B_Peer_Transmit_Result SendPod(char* pPod, size_t cbPod);

#ifdef _WINDOWS
		B_Peer_Transmit_Result OutterSend(void* pObj, void* p);
#endif

		/*
		* Return a value in the Handler (Worker Thread)
		*/
		bool Return(IBSerializable* pObj, void* p);
		bool ReturnPod(char* pPod, size_t cbPod);

#ifdef _WINDOWS
		bool OutterReturn(void* pObj, void* p);
#endif


		// Data-Transmitting

		BDataInfo* GetDataInfo(int nID);

		B_Peer_Transmit_Result InitSendFile(LPCTSTR lpszFile, void* pHeader, int& nID);
		B_Peer_Transmit_Result InitRecvFile(int nID, unsigned long long cb, LPCTSTR lpszFile);

		bool ClientSendFile(int nID);
		bool ClientSendFileFinal(int nID);

		B_Peer_Transmit_Result ClientRecvFile(int nID, IBSerializable* pObj, void* p);
		B_Peer_Transmit_Result ClientRecvFileFinal(int nID);
		bool ClientRecvFileData(int nID, BMemoryBrick* pBrick, size_t nPos);

		bool ServerRecvFile(int nID, BMemoryBrick* pBrick, size_t nPos);
		bool ServerSendFile(int nID);

		void TerminateTransmitting(int nID);
		void TerminateAllTransmittings(BPeerDataInfo** ppPDI = NULL);

#ifndef _WINDOWS
		int GetOptError();
#endif


		void Clear();
		void HandleError(BCallingSite callingSite, ssize_t err);

		bool Shutdown(bool bRead = true, bool bWrite = true);

#ifdef _WINDOWS
		int BeginAccept(BPeer* peer);
		void EndAccept(BPeer* pListen);

		int BeginConnect(PSOCKADDR_IN psa);
		void EndConnect();

		int BeginDisconnect();
		void EndDisconnect();

		/*
		* 0 : success
		* Others: winerror
		*/
		int BeginRead();
		void EndRead(DWORD cb);

		/*
		* 0 : success
		* Others: winerror
		*/
		int BeginWrite();
		void EndWrite(DWORD cb);

		//int BeginSysTransmitFile();
		//void EndSysTransmitFile(DWORD cb);

		int BeginTransmitFile();
		void EndTransmitFile(DWORD cb);

		bool UpdateContext();
		void GetAcceptedPeerAddress(PSOCKADDR_IN* ppsaLocal, PSOCKADDR_IN* ppsaRemote);
		void GetConnectedPeerAddress(PSOCKADDR_IN psaLocal);
#else
		/*
		* 0 : success
		* -1: failed
		* errno
		*      EAGAIN means accept has been done
		*
		* auto-join-the-circle-when-success-or-failed
		* auto-join-the-circle-for-accepted-peer
		*/
		int Accept();

		/*
		* 0 : success
		* errno
		*      EINPROGRESS means connect-pending
		*/
		int Connect(sockaddr_in* psa);

		/*
		* 0 : success, reading now
		* -1 : peer shutdown
		* errno
		*      EAGAIN means read has been done
		*
		* auto-join-the-circle-when-EAGAIN-and-parallel-mode
		* push-work-when-success
		*/
		ssize_t Read();

		/*
		* 0 : success
		* -1 : failed
		* errno
		*      EAGAIN means write buffer is full or something else
		*
		* auto-join-the-circle-for-writing-when-EAGAIN
		*/
		ssize_t Write();

		/*
		* Write pending
		*/
		void WriteAgain();
#endif


		/*
		* Create server
		*/
		bool ActAsServer(sockaddr_in* psa, int nBacklog = 64);

		/*
		* Create Client
		*/
		bool ActAsClient(sockaddr_in* psa);

		/*
		* time in seconds
		*/
		void Linger(int nLinger);

		/*
		* Call handler to parse incoming data
		*
		* auto-join-the-circle-when-!retire-and-!close-and-serial-mode
		*/
		void Craft();

		/*
		* Close socket, call Dispatcher's Reclaim-Peer-Method
		*/
		void Close();


#ifndef _WINDOWS
		bool SetOptNonBlocking();
#endif
		bool SetOptReuse();

		bool Create(B_Sock_Type sockType, BIOExecutionUnit* pExecUnit, BIOHandler* pHandler, IBDataChannel* pProtocol, B_IO_Operation operation);
		bool CreateAccept(BPeer* peer);

#ifndef _WINDOWS
		bool CreateFrom(int s, B_Sock_Type sockType, BIOExecutionUnit* pExecUnit);
#endif

		void Destroy(bool bNow = true, unsigned int flags = 0);
		void DestroyUnavailability();

		void SetConnected();
		//void RemoveFromCollection();

#ifndef _WINDOWS
		bool HandleCompatibleInitialization();
#endif

		// For .Net async/await compatibility
		
		BMemoryBrick* MoveOutBrick();
		void MoveInBrick(BMemoryBrick* pBrick);


		// Override

		virtual void ConfigBehaviorArbiter();
		virtual BIOBehaviorArbiter* GetBehaviorArbiter();


		inline void SetServerID(const unsigned int nCookie) { m_nServerID = nCookie; }
		inline void SetID(const unsigned int nID) { m_nID = nID; }

		inline void SetServer(BServerPeer* pServer) { m_pServer = pServer; }		

		bool CreateStreamSocket()
		{
#ifdef _WINDOWS
			m_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			return INVALID_SOCKET != m_s;
#else
			m_s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			return -1 != m_s;
#endif
		}

		bool CreateDatagramSocket()
		{
#ifdef _WINDOWS
			m_s = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
			return INVALID_SOCKET != m_s;
#else
			m_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
			return -1 != m_s;
#endif
		}

		bool CreateUnixDomainStreamSocket()
		{
#ifdef _WINDOWS
			m_s = WSASocket(AF_UNIX, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			return INVALID_SOCKET != m_s;
#else
			m_s = socket(AF_UNIX, SOCK_STREAM, 0);
			return -1 != m_s;
#endif
		}

		bool CreateUnixDomainDatagramSocket()
		{

#ifdef _WINDOWS
			m_s = WSASocket(AF_UNIX, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
			return INVALID_SOCKET != m_s;
#else
			m_s = socket(AF_UNIX, SOCK_DGRAM, 0);
			return -1 != m_s;
#endif
		}

		inline void NotifyConnected()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_CONNECTED, NULL, NULL);
		}

		inline bool NotifyAccepted()
		{
			return m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_ACCEPTED, NULL, NULL);
		}

		inline void NotifyConnectRefused()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_CONNECT_REFUSED, NULL, NULL);
		}

		inline void NotifyNetNameUnavailable()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_NETNAME_UNAVAILABLE, NULL, NULL);
		}

		inline void NotifyHostUnavailable()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_HOST_UNAVAILABLE, NULL, NULL);
		}

		inline void NotifyNetUnavailable()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_UNAVAILABLE, NULL, NULL);
		}

		inline void NotifyConnectionReset()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_CONNECTION_RESET, NULL, NULL);
		}

		inline bool NotifyOutterCreate(LPB_Sock_Outter_Create param)
		{
			return m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_OUTTERCREATE, param, NULL);			
		}

		inline void NotifyMaliciousConnection(B_Malicious_Connection_Type type, void* p)
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NET_MALICIOUS_CONNECTION, (void*)type, p);
		}

	};


}


#endif // _I_PEER_H_
