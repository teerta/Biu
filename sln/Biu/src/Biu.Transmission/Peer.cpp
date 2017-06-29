#include "stdafx.h"
#include "Peer.h"
#include "baq.h"
#include "bpeerrepre.h"
#include "ServerPeer.h"
#include "PeerStateKeeper.h"


using namespace System::Runtime::InteropServices;
using namespace Biu::Transmission;


Peer::Peer()
	: m_peer(NULL)
{
}

Peer::~Peer()
{

}

Peer::!Peer()
{

}

PeerTransmitResult Peer::Send(IBiuSerializable^ obj, AshleyHeader^ header)
{
	GCHandle hObj;
	GCHandle hHeader;
	try
	{
		hObj = GCHandle::Alloc(obj);
		hHeader = GCHandle::Alloc(header);
		return (PeerTransmitResult)m_peer->OutterSend(GCHandle::ToIntPtr(hObj).ToPointer(), GCHandle::ToIntPtr(hHeader).ToPointer());
	}
	catch (Exception^)
	{
		throw;
	}
	finally
	{
		hObj.Free();
		hHeader.Free();
	}
}

bool Peer::Return(IBiuSerializable^ obj, AshleyHeader^ header)
{
	GCHandle hObj;
	GCHandle hHeader;
	try
	{
		hObj = GCHandle::Alloc(obj);
		hHeader = GCHandle::Alloc(header);
		return m_peer->OutterReturn(GCHandle::ToIntPtr(hObj).ToPointer(), GCHandle::ToIntPtr(hHeader).ToPointer());
	}
	catch (Exception^)
	{
		throw;
	}
	finally
	{
		hObj.Free();
		hHeader.Free();
	}
}

bool Peer::Return(IBiuSerializable^ obj, AshleyHeader^ header, PeerStateKeeper^ peerStateKeeper)
{
	try
	{
		if (nullptr != peerStateKeeper)
		{
			peerStateKeeper->Restore();
		}

		return Return(obj, header);
	}
	catch (Exception^)
	{
		throw;
	}	
}

DataInfo^ Peer::GetDataInfo(int nID)
{
	auto* p = m_peer->GetDataInfo(nID);
	if (NULL == p)
	{
		return nullptr;
	}

	DataInfo^ di = gcnew DataInfo();
	di->Inner = p;

	return di;
}

PeerTransmitResult Peer::InitSendFile(String^ file, AshleyHeader^ header, int% id)
{
	LPB_ASHLEY_HEADER lpHeader = NULL;
	B_Peer_Transmit_Result result = BPTR_True;
	try
	{
		lpHeader = new B_ASHLEY_HEADER();
		header->ToNativeHeader(lpHeader);

		int nID = 0;
		pin_ptr<const wchar_t> pszFile = PtrToStringChars(file);
		result = m_peer->InitSendFile(pszFile, lpHeader, nID);

		id = nID;

		return (PeerTransmitResult)result;
	}
	catch (Exception^)
	{
		throw;
	}
	finally
	{
		if (BPTR_True != result)
		{
			delete lpHeader;
		}
	}
}

PeerTransmitResult Peer::InitRecvFile(int id, unsigned long long size, String^ file)
{
	try
	{
		pin_ptr<const wchar_t> pszFile = PtrToStringChars(file);
		B_Peer_Transmit_Result result = m_peer->InitRecvFile(id, size, pszFile);
		
		return (PeerTransmitResult)result;
	}
	catch (Exception^)
	{
		throw;
	}
}

bool Peer::ClientSendFile(int id)
{
	try
	{
		return m_peer->ClientSendFile(id);
	}
	catch (Exception^)
	{
		throw;
	}
}

bool Peer::ClientSendFileFinal(int id)
{
	try
	{
		return m_peer->ClientSendFileFinal(id);
	}
	catch (Exception^)
	{
		throw;
	}
}

PeerTransmitResult Peer::ClientRecvFile(int id, unsigned long long origin, unsigned long long size, AshleyHeader^ header)
{
	LPB_ASHLEY_HEADER lpHeader = NULL;
	Biu_RecvFile* pObj = NULL;
	B_Peer_Transmit_Result result = BPTR_True;
	try
	{
		lpHeader = new B_ASHLEY_HEADER();
		header->ToNativeHeader(lpHeader);

		pObj = new Biu_RecvFile();
		pObj->m_ID = id;
		pObj->m_Origin = origin;
		pObj->m_Size = size;
		result = m_peer->ClientRecvFile(id, pObj, lpHeader);

		return (PeerTransmitResult)result;
	}
	catch (Exception^)
	{
		throw;
	}
	finally
	{
		if (!(BPTR_True == result || BPTR_Pending == result))
		{
			delete lpHeader;
			delete pObj;
		}
	}
}

PeerTransmitResult Peer::ClientRecvFileFinal(int id)
{
	try
	{
		return (PeerTransmitResult)m_peer->ClientRecvFileFinal(id);
	}
	catch (Exception^)
	{
		throw;
	}
}

bool Peer::ClientRecvFileData(int id, MemoryBrick^ brick, size_t pos)
{
	try
	{
		return m_peer->ClientRecvFileData(id, brick->Brick, pos);
	}
	catch (Exception^)
	{
		throw;
	}
}

bool Peer::ServerRecvFile(int id, MemoryBrick^ brick, size_t pos)
{
	try
	{
		return m_peer->ServerRecvFile(id, brick->Brick, pos);
	}
	catch (Exception^)
	{
		throw;
	}
}

bool Peer::ServerSendFile(int id)
{
	try
	{
		return m_peer->ServerSendFile(id);
	}
	catch (Exception^)
	{
		throw;
	}
}

void Peer::TerminateTransmitting(int id)
{
	try
	{
		m_peer->TerminateTransmitting(id);
	}
	catch (Exception^)
	{
		throw;
	}
}

void Peer::TerminateAllTransmittings()
{
	try
	{
		m_peer->TerminateAllTransmittings();
	}
	catch (Exception^)
	{
		throw;
	}
}

void Peer::UpdateRepresentation()
{	
	if (m_peer->GetRepresentation()->GetID())
	{
		m_endPoint = gcnew IPEndPoint(m_peer->GetRepresentation()->GetAddressLong(), m_peer->GetRepresentation()->GetPort());

		//char sz[B_MAX_NAME_SIZE] = { 0 };		 
		//m_peer->GetRepresentation()->GetAddress(sz, B_MAX_NAME_SIZE);
		//auto str = gcnew String(sz);

		//IPAddress^ address;
		//if (IPAddress::TryParse(str, address))
		//{
		//	this->Representation = gcnew IPEndPoint(m_peer->GetRepresentation()->GetAddressLong(), m_peer->GetRepresentation()->GetPort());
		//}
	}

	if (m_peer->GetRemoteRepresentation()->GetID())
	{
		m_remoteEndPoint = gcnew IPEndPoint(m_peer->GetRemoteRepresentation()->GetAddressLong(), m_peer->GetRemoteRepresentation()->GetPort());

		//char sz[B_MAX_NAME_SIZE] = { 0 };		
		//m_peer->GetRemoteRepresentation()->GetAddress(sz, B_MAX_NAME_SIZE);
		//auto str = gcnew String(sz);

		//IPAddress^ address;
		//if (IPAddress::TryParse(str, address))
		//{
		//	this->RemoteRepresentation = gcnew IPEndPoint(address, m_peer->GetRemoteRepresentation()->GetPort());
		//}
	}

	//try
	//{
	//	
	//}
	//catch (Exception^)
	//{
	//	this->Representation = nullptr;
	//	ZeroMemory(sz, B_MAX_NAME_SIZE);
	//}

	//try
	//{
	//	
	//}
	//catch (Exception^)
	//{
	//	this->RemoteRepresentation = nullptr;
	//}
}

ServerPeer^ Peer::Server::get()
{
	ServerPeer^ p = gcnew ServerPeer();
	p->SetServer(m_peer->GetServer());

	return p;
}

Ashley^ Peer::Protocol::get()
{
	Ashley^ p = gcnew Ashley();
	p->AshleyProtocol = static_cast<BAshley*>(m_peer->GetProtocol());

	return p;
}

long long Peer::Var::get()
{
	return m_peer->GetVar();
}

IOSecurityContext^ Peer::SecurityContext::get()
{
	auto* p = m_peer->GetSecurityContext();
	if (!p)
	{
		return nullptr;
	}

	IOSecurityContext^ pObj = gcnew IOSecurityContext();
	pObj->m_pInner = m_peer->GetSecurityContext();

	return pObj;
}

SocketTrait Peer::Trait::get()
{
	return (SocketTrait)m_peer->GetTrait();
}

BMemoryBrick* Peer::MoveOutBrick()
{
	return m_peer->MoveOutBrick();
}

void Peer::MoveInBrick(BMemoryBrick* pBrick)
{
	m_peer->MoveInBrick(pBrick);
}
