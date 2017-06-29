#pragma once


#include "bpeersvr.h"
#include "Enum.h"
#include "Peer.h"


using namespace System;
using namespace Biu;


namespace Biu { namespace Transmission {
	

	// Ref Class ServerPeer

	public ref class ServerPeer
	{
	public:
		ServerPeer();

		Peer^ __clrcall GetPeerByKey(unsigned long long key);
		Peer^ __clrcall GetPeerByID(unsigned int id);
		

		property Peer^ Server
		{
			Peer^ __clrcall get() { return m_peer; }
		}

		property bool IsRunning
		{
			bool __clrcall get() { return m_serverPeer->IsRunning(); }
		}

		property IOReadMode Mode
		{
			IOReadMode __clrcall get() { return (IOReadMode)m_serverPeer->GetMode(); }
			void __clrcall set(IOReadMode value) { m_serverPeer->SetMode((B_IO_Read_Mode)value); }
		}

	internal:
		void SetServer(BServerPeer* p)
		{
			m_serverPeer = p;
			m_peer = gcnew Peer();
			m_peer->InnerPeer = m_serverPeer->GetServerPeer();
		}

	private:
		BServerPeer*		m_serverPeer;
		Peer^				m_peer;
	};


} }
