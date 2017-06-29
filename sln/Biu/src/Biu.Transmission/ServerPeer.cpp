#include "stdafx.h"
#include "ServerPeer.h"


using namespace Biu::Transmission;


ServerPeer::ServerPeer()
{
}

Peer^ ServerPeer::GetPeerByKey(unsigned long long key)
{	
	auto* p = m_serverPeer->GetPeer(reinterpret_cast<BPeerRepresentation*>(key));
	if (!p)
	{
		return nullptr;
	}

	Peer^ peer = gcnew Peer();
	peer->InnerPeer = p;

	return peer;
}

Peer^ ServerPeer::GetPeerByID(unsigned int id)
{
	auto* p = m_serverPeer->GetPeer(id);
	if (!p)
	{
		return nullptr;
	}

	Peer^ peer = gcnew Peer();
	peer->InnerPeer = p;

	return peer;
}
