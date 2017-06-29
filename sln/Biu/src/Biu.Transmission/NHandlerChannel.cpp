#include "stdafx.h"
#include "NHandlerChannel.h"
#include "IOHandler.h"
#include "Peer.h"
#include "Ashley.h"
#include "MemoryShack.h"
#include "MemoryBrick.h"
#include "Serializer.h"
#include "PeerData.h"
#include "bpeer.h"
#include "bashley.h"


using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Biu;
using namespace Biu::Transmission;


NHandlerChannel::NHandlerChannel()
	: m_pHandler(NULL)
{
}


NHandlerChannel::~NHandlerChannel()
{
}

bool NHandlerChannel::HandleIo(
	BIODispatchable* pDisp,
	unsigned long long action,
	LPB_IO_Operation pOperation
	)
{
	CHK_false(m_pHandler);

	BPeer* nativePeer = static_cast<BPeer*>(pDisp);
	CHK_false(nativePeer);

	BAshley* protocol = static_cast<BAshley*>(nativePeer->GetProtocol());
	CHK_false(protocol);	

	GCHandle hHandler = GCHandle::FromIntPtr(IntPtr(m_pHandler));
	IOHandler^ handler = static_cast<IOHandler^>(hHandler.Target);
	if (nullptr == handler)
	{
		return false;
	}

	Peer^ peer = gcnew Peer();
	peer->InnerPeer = nativePeer;

	IOOperation operation = (IOOperation)(*pOperation);

	bool bRet = handler->OnIO(action, peer, operation);
	*pOperation = (B_IO_Operation)(unsigned int)operation;

	return bRet;
}

bool NHandlerChannel::HandleCmd(
	BIODispatchable* pDisp,
	unsigned long long cmd,
	void* p1,
	void* p2
	)
{
	CHK_false(m_pHandler);

	BPeer* nativePeer = static_cast<BPeer*>(pDisp);
	CHK_false(nativePeer);	

	GCHandle hHandler = GCHandle::FromIntPtr(IntPtr(m_pHandler));
	IOHandler^ handler = static_cast<IOHandler^>(hHandler.Target);
	if (nullptr == handler)
	{
		return false;
	}

	auto* pShack = handler->Shack->m_pShack;

	Peer^ peer = gcnew Peer();
	peer->InnerPeer = nativePeer;

	bool bRet = true;
	switch (cmd)
	{
	case BIO_CMD_NEW_HANDLE:
	{
		BAshley* pProtocol = static_cast<BAshley*>(nativePeer->GetProtocol());
		assert(pProtocol);

		pProtocol->SetMemoryModel(pShack, BReadBrickSparsely_char);
	}
		break;

	case BIO_CMD_NET_OUTTERCREATE:
	{
		LPB_Sock_Outter_Create param = reinterpret_cast<LPB_Sock_Outter_Create>(p1);
		
		GCHandle hSerObj = GCHandle::FromIntPtr(IntPtr(param->pObj));
		IBiuSerializable^ serObj = static_cast<IBiuSerializable^>(hSerObj.Target);

		GCHandle hHeader = GCHandle::FromIntPtr(IntPtr(param->pHeader));
		AshleyHeader^ header = static_cast<AshleyHeader^>(hHeader.Target);

		MemoryBrick^ brick = gcnew MemoryBrick();
		brick->Brick = param->pBrick;

		bRet = peer->Protocol->CreateObject(brick, serObj, header, peer->SecurityContext);
	}
		break;

	case BIO_CMD_WRITE_DONE:
		handler->Command->OnWriteDone(peer, reinterpret_cast<long>(p1));
		break;

	case BIO_CMD_NET_CONNECTED:
		handler->Command->OnConnected(peer);
		break;

	case BIO_CMD_NET_ACCEPTED:
	{
		auto arg = gcnew BoolArg();
		handler->Command->OnAccepted(peer, arg);
		bRet = arg->RetuenValue;
	}		
		break;

	case BIO_CMD_CLOSE_PROACTIVE:
		handler->Command->OnCloseProactive(peer);
		break;

	case BIO_CMD_CLOSE_PASSIVE:
		handler->Command->OnClosePassive(peer);
		break;

	case BIO_CMD_NET_CONNECT_REFUSED:
		handler->Command->OnConnectedRefused(peer);
		break;

	case BIO_CMD_NET_NETNAME_UNAVAILABLE:
		handler->Command->OnNetNameUnavailable(peer);
		break;

	case BIO_CMD_NET_HOST_UNAVAILABLE:
		handler->Command->OnHostUnavailable(peer);
		break;

	case BIO_CMD_NET_CONNECTION_RESET:
		handler->Command->OnConnectionReset(peer);
		break;

	case BIO_CMD_NET_UNAVAILABLE:
		handler->Command->OnNetAvailable(peer);
		break;

	case BIO_CMD_TIMEOUT:
		handler->Command->OnTimeout(peer);
		break;

	case BIO_CMD_ERROR:
		handler->Command->OnError(peer, reinterpret_cast<unsigned long long>(p1), reinterpret_cast<long>(p2));
		break;

	case BIO_CMD_DATA_HEADER:
	{
		//DataInfo^ dataInfo = gcnew DataInfo();
		//dataInfo->Inner = reinterpret_cast<BDataInfo*>(p1);

		//handler->Command->OnDataHeader(peer, dataInfo);
		BDataInfo* pDI = reinterpret_cast<BDataInfo*>(p1);
		LPB_ASHLEY_HEADER lpHeader = reinterpret_cast<LPB_ASHLEY_HEADER>(pDI->GetHeader());
		lpHeader->Pa.ID = pDI->GetID();
		lpHeader->Pa.Total = pDI->GetSize();
		lpHeader->Pa.Current = pDI->GetFinished();
	}
		break;

	case BIO_CMD_FILE:
	{
		DataInfo^ dataInfo = gcnew DataInfo();
		dataInfo->Inner = reinterpret_cast<BDataInfo*>(p2);

		handler->Command->OnFile(peer, dataInfo);
	}
		break;

	case BIO_CMD_FILE_NEXT:
	{
		BDataContinuation* nativeContinuation = reinterpret_cast<BDataContinuation*>(p1);
		BWaitDataInfo* nativeWaitDataInfo = reinterpret_cast<BWaitDataInfo*>(p2);

		DataContinuation^ continuation = gcnew DataContinuation();
		continuation->Create(nativeContinuation);

		WaitDataInfo^ waitDataInfo = nullptr;
		if (nativeWaitDataInfo)
		{
			waitDataInfo = gcnew WaitDataInfo();
			waitDataInfo->Inner = nativeWaitDataInfo;
		}
		

		handler->Command->OnFileNext(peer, continuation, waitDataInfo);
	}
		break;

	default:
		bRet = false;
	}

	return bRet;
}
