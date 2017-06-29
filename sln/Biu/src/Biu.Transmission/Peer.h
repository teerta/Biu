#pragma once


#include "bpeer.h"
#include "Enum.h"
#include "Ashley.h"
#include "MemoryBrick.h"
#include "QuickObject.h"
#include "PeerData.h"
#include "IOSecurityContext.h"


using namespace System;
using namespace System::Net;
using namespace Biu;


namespace Biu { namespace Transmission {


	ref class ServerPeer;
	ref class PeerStateKeeper;


	// Ref Class Peer

	public ref class Peer
	{
	public:
		Peer();
		~Peer();
		!Peer();

		PeerTransmitResult __clrcall Send(IBiuSerializable^ obj, AshleyHeader^ header);
		bool __clrcall Return(IBiuSerializable^ obj, AshleyHeader^ header);
		bool __clrcall Return(IBiuSerializable^ obj, AshleyHeader^ header, PeerStateKeeper^ peerStateKeeper);

		DataInfo^ __clrcall GetDataInfo(int nID);

		PeerTransmitResult __clrcall InitSendFile(String^ file, AshleyHeader^ header, int% id);
		PeerTransmitResult __clrcall InitRecvFile(int id, unsigned long long size, String^ file);

		bool __clrcall ClientSendFile(int id);
		bool __clrcall ClientSendFileFinal(int id);

		PeerTransmitResult __clrcall ClientRecvFile(int id, unsigned long long origin, unsigned long long size, AshleyHeader^ header);
		PeerTransmitResult __clrcall ClientRecvFileFinal(int id);
		bool __clrcall ClientRecvFileData(int id, MemoryBrick^ brick, size_t pos);

		bool __clrcall ServerRecvFile(int id, MemoryBrick^ brick, size_t pos);
		bool __clrcall ServerSendFile(int id);

		void __clrcall TerminateTransmitting(int id);
		void __clrcall TerminateAllTransmittings();


		property IPEndPoint^ Representation
		{
			IPEndPoint^ __clrcall get() { return m_endPoint; }
		}

		property IPEndPoint^ RemoteRepresentation
		{
			IPEndPoint^ __clrcall get() { return m_remoteEndPoint; }
		}

		property SocketType SockType
		{
			SocketType __clrcall get() { return (SocketType)m_peer->GetPeerSockType(); }
		}

		property bool IsServer
		{
			bool __clrcall get() { return m_peer->IsServer(); }
		}

		property unsigned int ID
		{
			unsigned int __clrcall get() { return m_peer->GetID(); }
		}

		property unsigned int ServerID
		{
			unsigned int __clrcall get() { return m_peer->GetServerID(); }
		}

		property unsigned int ReferenceID
		{
			unsigned int __clrcall get() { return m_peer->GetRefID(); }
			void __clrcall set(unsigned int value) { m_peer->SetRefID(value); }
		}

		/// <summary>
		/// For Server-Peer (Listen-Peer) and Client-Peer
		/// </summary>
		property unsigned long long Key
		{
			unsigned long long __clrcall get() { return reinterpret_cast<unsigned long long>(m_peer->GetRepresentation()); }
		}

		/// <summary>
		/// For Server-Accepted-Peer
		/// </summary>
		property unsigned long long RemoteKey
		{
			unsigned long long __clrcall get() { return reinterpret_cast<unsigned long long>(m_peer->GetRemoteRepresentation()); }
		}

		property ServerPeer^ Server
		{
			ServerPeer^ __clrcall get();
		}

		property Ashley^ Protocol
		{
			Ashley^ __clrcall get();
		}

		property long long Var
		{
			long long __clrcall get();
		}

		property IOSecurityContext^ SecurityContext
		{
			IOSecurityContext^ __clrcall get();
		}

		property SocketTrait Trait
		{
			SocketTrait __clrcall get();
		}

	internal:
		property BPeer* InnerPeer
		{
			BPeer* get()
			{
				return m_peer;
			}

			void set(BPeer* val)
			{
				m_peer = val;
				UpdateRepresentation();
			}
		}

		BMemoryBrick* MoveOutBrick();
		void MoveInBrick(BMemoryBrick* pBrick);


	private:
		BPeer*				m_peer;
		IPEndPoint^			m_endPoint;
		IPEndPoint^			m_remoteEndPoint;


		void UpdateRepresentation();
	};


}}
