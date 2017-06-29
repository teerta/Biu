
#ifndef _BPEER_H_
#define _BPEER_H_


#include "biu.h"
#include "bndef.h"
#include "biodef.h"
#include "biodisp.h"
#include "biohand.h"




namespace Biu {


	// Forwarding

	class BServerPeer;
	class BDataPeer;
	class BDataInfo;
	class BPeerRepresentation;
	class BPeerDataInfo;

#ifdef _WINDOWS
	class BSockExMethod;
#endif


	// Class BPeer

	class BIU_API BPeer : public BIODispatchable
	{
	public:
		BPeer();
		BPeer(const BPeer& peer) = delete;
		virtual ~BPeer();
		
		BPeer& operator = (const BPeer& peer) = delete;
		bool operator ==(const BPeer& peer) = delete;
		bool operator !=(const BPeer& peer) = delete;

#ifdef _WINDOWS
		virtual HANDLE GetCircleHandle();
#else
		virtual int GetCircleHandle();
#endif

		virtual void HandleError(BCallingSite callingSite, ssize_t err);
		virtual void Clear();

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
		
		void SetSendFlags(int nFlags);
		void SetRecvFlags(int nFlags);

		B_Sock_Type GetPeerSockType();
		BPeerRepresentation* GetRepresentation();
		BPeerRepresentation* GetRemoteRepresentation();

		bool IsServer();

		unsigned int GetID();
		unsigned int GetServerID();
		BServerPeer* GetServer();
		
		void SetRefID(unsigned int nID);
		unsigned int GetRefID();

		B_Sock_Trait GetTrait();
		void* GetTraitVal();

		BMemoryBrick* MoveOutBrick();
		void MoveInBrick(BMemoryBrick* pBrick);
		
//#ifdef _WINDOWS
//		operator HANDLE() { return (HANDLE)m_s; }
//#else
//		operator int() { return m_s; }
//#endif
	};


}


#endif // _BPEER_H_
