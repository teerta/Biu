#include "stdafx.h"
#include "NetIOExecutionUnit.h"
#include "Peer.h"
#include "IOHandler.h"
#include "ServerPeer.h"
#include "PeerData.h"
#include "Ashley.h"
#include "bpeer.h"
#include "bpeersvr.h"
#include "bpeerdata.h"


using namespace Biu;
using namespace Biu::Transmission;


// ConnectionCreation

ConnectionCreation::ConnectionCreation()
{
	this->Trait = SocketTrait::Normal;
	this->Type = SocketType::IPv4TCP;
	this->AcceptCount = 10;
	this->Timeout = -1;
	this->Backlog = 64;
	this->CompatibleInitializationState = false;
	this->Var = 0;
	this->Ref = 0;
}

void ConnectionCreation::ToNative(BNConnectionCreation* pncc)
{
	pncc->trait = (B_Sock_Trait)this->Trait;
	pncc->sockType = (B_Sock_Type)this->Type;
	pncc->pProtocol = this->Protocol->AshleyProtocol;
	pncc->pHandler = this->Handler->m_pInner;
	pncc->nAccepts = this->AcceptCount;
	pncc->llTimeoutSeconds = this->Timeout;
	pncc->nBacklog = this->Backlog;
	pncc->llVar = this->Var;
	pncc->nRef = this->Ref;
	pncc->bCompatibleInitializationState = this->CompatibleInitializationState;
	if (nullptr != this->EndPoint)
	{
		pin_ptr<const wchar_t> pszAddr = PtrToStringChars(this->EndPoint->Address->ToString());
		pncc->sa.sin_family = AF_INET;
		pncc->sa.sin_port = ntohs(this->EndPoint->Port);
		pncc->sa.sin_addr.S_un.S_addr = inet_addr(CW2A(pszAddr));
	}

	if (nullptr != this->RemoteEndPoint)
	{
		pin_ptr<const wchar_t> pszAddr = PtrToStringChars(this->RemoteEndPoint->Address->ToString());
		pncc->saRemote.sin_family = AF_INET;
		pncc->saRemote.sin_port = ntohs(this->RemoteEndPoint->Port);
		pncc->saRemote.sin_addr.S_un.S_addr = inet_addr(CW2A(pszAddr));
	}

	if (nullptr != this->SecurityProvider)
	{
		pncc->pSec = this->SecurityProvider->m_pInner;
	}
}


// NetIOExecutionUnit

NetIOExecutionUnit::NetIOExecutionUnit()
	: m_pUnit(NULL)
	, m_bDetached(false)
{
	m_pUnit = new BNetIOExecutionUnit();
}

NetIOExecutionUnit::~NetIOExecutionUnit()
{
	this->!NetIOExecutionUnit();
}

NetIOExecutionUnit::!NetIOExecutionUnit()
{
	if (!m_bDetached)
	{
		SAFE_DELETE(m_pUnit);
	}
}

bool NetIOExecutionUnit::Listen(ConnectionCreation^ cc, unsigned int% cookie)
{
	//pin_ptr<const wchar_t> pszAddr = PtrToStringChars(cc->EndPoint->Address->ToString());
	//SOCKADDR_IN sa = { 0 };
	//sa.sin_family = AF_INET;
	//sa.sin_port = ntohs(cc->EndPoint->Port);
	//sa.sin_addr.S_un.S_addr = inet_addr(CW2A(pszAddr));

	unsigned int nCookie = cookie;
	BNConnectionCreation ncc;
	cc->ToNative(&ncc);

	bool bRet = m_pUnit->Listen(ncc, nCookie);

	cookie = nCookie;

	return bRet;
}

bool NetIOExecutionUnit::Connect(ConnectionCreation^ cc, unsigned int% cookie)
{
	//pin_ptr<const wchar_t> pszAddr = PtrToStringChars(cc->EndPoint->Address->ToString());
	//SOCKADDR_IN sa = { 0 };
	//sa.sin_family = AF_INET;
	//sa.sin_port = ntohs(cc->EndPoint->Port);
	//sa.sin_addr.S_un.S_addr = inet_addr(CW2A(pszAddr));

	//pszAddr = PtrToStringChars(cc->RemoteEndPoint->Address->ToString());
	//SOCKADDR_IN saRemote = { 0 };
	//saRemote.sin_family = AF_INET;
	//saRemote.sin_port = ntohs(cc->RemoteEndPoint->Port);
	//saRemote.sin_addr.S_un.S_addr = inet_addr(CW2A(pszAddr));

	unsigned int nCookie = 0;
	BNConnectionCreation ncc;
	cc->ToNative(&ncc);

	bool bRet = m_pUnit->Connect(ncc, nCookie);

	cookie = nCookie;

	return bRet;
}

bool NetIOExecutionUnit::ConnectData(SocketTrait trait, unsigned int cookie)
{
	return m_pUnit->ConnectData((B_Sock_Trait)trait, cookie);
}

Peer^ NetIOExecutionUnit::AcquirePeerForConnecting(ConnectionCreation^ cc, unsigned int% cookie)
{
	unsigned int nCookie = 0;
	BNConnectionCreation ncc;
	cc->ToNative(&ncc);

	auto* peer = m_pUnit->AcquirePeerForConnecting(ncc, nCookie);
	if (!peer)
	{
		return nullptr;
	}

	cookie = nCookie;

	Peer^ p = gcnew Peer();
	p->InnerPeer = peer;

	return p;
}

bool NetIOExecutionUnit::Connect(Peer^ peer, ConnectionCreation^ cc, unsigned int% cookie)
{
	if (!peer->InnerPeer || 0 == cookie)
	{
		return false;
	}

	unsigned int nCookie = cookie;
	BNConnectionCreation ncc;
	cc->ToNative(&ncc);

	return m_pUnit->Connect(peer->InnerPeer, ncc, nCookie);
}

void NetIOExecutionUnit::CloseServer(unsigned int cookie)
{
	m_pUnit->CloseServer(cookie);
}

void NetIOExecutionUnit::CloseServerItem(unsigned int cookie, long long key)
{
	m_pUnit->CloseServerItem(cookie, reinterpret_cast<BPeerRepresentation*>(key));
}

void NetIOExecutionUnit::CloseClient(unsigned int cookie, bool now)
{
	m_pUnit->CloseClient(cookie, now);
}

bool NetIOExecutionUnit::ContainsServer(unsigned int cookie)
{
	return m_pUnit->ContainsServer(cookie);
}

bool NetIOExecutionUnit::ContainsClient(unsigned int cookie)
{
	return m_pUnit->ContainsClient(cookie);
}

ServerPeer^ NetIOExecutionUnit::GetServer(unsigned int cookie)
{
	auto* pServer = m_pUnit->GetServer(cookie);
	if (!pServer)
	{
		return nullptr;
	}

	ServerPeer^ p = gcnew ServerPeer();
	p->SetServer(pServer);

	return p;
}

Peer^ NetIOExecutionUnit::GetClient(unsigned int cookie)
{
	auto* pClient = m_pUnit->GetClient(cookie);
	if (!pClient)
	{
		return nullptr;
	}

	Peer^ p = gcnew Peer();
	p->InnerPeer = pClient;

	return p;
}

DataPeer^ NetIOExecutionUnit::GetData(unsigned int cookie)
{
	auto* pData = m_pUnit->GetData(cookie);
	if (!pData)
	{
		return nullptr;
	}

	DataPeer^ p = gcnew DataPeer();
	p->Inner = pData;

	return p;
}

ClientDataPeer^ NetIOExecutionUnit::GetClientData(unsigned int cookie)
{
	auto* pData = m_pUnit->GetClientData(cookie);
	if (!pData)
	{
		return nullptr;
	}

	ClientDataPeer^ p = gcnew ClientDataPeer();
	p->Inner = pData;

	return p;
}
