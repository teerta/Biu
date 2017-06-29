
#include "stdafx.h"
#include "PeerStateKeeper.h"


using namespace Biu::Transmission;


PeerStateKeeper::PeerStateKeeper(Peer^ peer)
	: m_peer(peer)
	, m_pBrick(__VNULL)
{
	if (nullptr != peer)
	{
		m_pBrick = m_peer->MoveOutBrick();
	}	
}

PeerStateKeeper::~PeerStateKeeper()
{
	if (m_pBrick)
	{
		m_peer->MoveInBrick(m_pBrick);
	}

	m_peer = nullptr;
	this->!PeerStateKeeper();
}

PeerStateKeeper::!PeerStateKeeper()
{
	m_pBrick = __VNULL;
}

void Biu::Transmission::PeerStateKeeper::Restore()
{
	if (m_pBrick)
	{
		m_peer->MoveInBrick(m_pBrick);
		m_pBrick = __VNULL;
	}
}