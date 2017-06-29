#include "stdafx.h"
#include "IOHandler.h"
#include "NHandlerChannel.h"
#include "PeerData.h"


using namespace Biu::Transmission;


// IOCommand

IOCommand::IOCommand()
{

}

void IOCommand::OnError(Peer^ peer, unsigned long long ecs, long error)
{
	if (nullptr != _error && 0 != _error->GetInvocationList()->Length)
	{
		_error(peer, ecs, error);
	}
}

void IOCommand::OnTimeout(Peer^ peer)
{
	if (nullptr != _timeout && 0 != _timeout->GetInvocationList()->Length)
	{
		_timeout(peer);
	}
}

void IOCommand::OnWriteDone(Peer^ peer, long size)
{
	if (nullptr != _writeDone && 0 != _writeDone->GetInvocationList()->Length)
	{
		_writeDone(peer, size);
	}
}

void IOCommand::OnCloseProactive(Peer^ peer)
{
	if (nullptr != _closeProactive && 0 != _closeProactive->GetInvocationList()->Length)
	{
		_closeProactive(peer);
	}
}

void IOCommand::OnClosePassive(Peer^ peer)
{
	if (nullptr != _closePassive && 0 != _closePassive->GetInvocationList()->Length)
	{
		_closePassive(peer);
	}
}

void IOCommand::OnFile(Peer^ peer, DataInfo^ dataInfo)
{
	if (nullptr != _file && 0 != _file->GetInvocationList()->Length)
	{
		_file(peer, dataInfo);
	}
}

void IOCommand::OnFileNext(Peer^ peer, DataContinuation^ continuation, WaitDataInfo^ waitDataInfo)
{
	if (nullptr != _fileNext && 0 != _fileNext->GetInvocationList()->Length)
	{
		_fileNext(peer, continuation, waitDataInfo);
	}
}

//void IOCommand::OnDataHeader(Peer^ peer, DataInfo^ dataInfo)
//{
//	if (nullptr != _dataHeader && 0 != _dataHeader->GetInvocationList()->Length)
//	{
//		_dataHeader(peer, dataInfo);
//	}
//}

void IOCommand::OnConnected(Peer^ peer)
{
	if (nullptr != _connected && 0 != _connected->GetInvocationList()->Length)
	{
		_connected(peer);
	}
}

void IOCommand::OnAccepted(Peer^ peer, BoolArg^ arg)
{
	if (nullptr != _accepted && 0 != _accepted->GetInvocationList()->Length)
	{
		_accepted(peer, arg);
	}
}

void IOCommand::OnConnectedRefused(Peer^ peer)
{
	if (nullptr != _connectRefused && 0 != _connectRefused->GetInvocationList()->Length)
	{
		_connectRefused(peer);
	}
}

void IOCommand::OnNetNameUnavailable(Peer^ peer)
{
	if (nullptr != _netNameUnavailable && 0 != _netNameUnavailable->GetInvocationList()->Length)
	{
		_netNameUnavailable(peer);
	}
}

void IOCommand::OnHostUnavailable(Peer^ peer)
{
	if (nullptr != _hostUnavailable && 0 != _hostUnavailable->GetInvocationList()->Length)
	{
		_hostUnavailable(peer);
	}
}

void IOCommand::OnNetAvailable(Peer^ peer)
{
	if (nullptr != _netUnavailable && 0 != _netUnavailable->GetInvocationList()->Length)
	{
		_netUnavailable(peer);
	}
}

void IOCommand::OnConnectionReset(Peer^ peer)
{
	if (nullptr != _connectionReset && 0 != _connectionReset->GetInvocationList()->Length)
	{
		_connectionReset(peer);
	}
}


// IOHandler

IOHandler::IOHandler(unsigned long long identification, unsigned int maxPacketSize, unsigned int minPacketSize)
	: m_pChannel(NULL)
	, m_pInner(NULL)
{
	m_cmd = gcnew IOCommand();
	m_pChannel = new NHandlerChannel();
	m_pInner = new Ashley_ChannelHandler(identification, maxPacketSize, minPacketSize);
	m_hThis = GCHandle::Alloc(this);	
	m_pChannel->m_pHandler = GCHandle::ToIntPtr(m_hThis).ToPointer();

	m_pInner->SetHandler(m_pChannel, static_cast<BChannelIOHandler>(&NHandlerChannel::HandleIo), static_cast<BChannelCommandHandler>(&NHandlerChannel::HandleCmd));
}

IOHandler::~IOHandler()
{
	m_hThis.Free();
	this->!IOHandler();
}

IOHandler::!IOHandler()
{
	SAFE_DELETE(m_pInner);
}

bool IOHandler::OnIO(unsigned long long action, Peer^ peer, IOOperation% operation)
{
	return this->IO(/*this->Channel, */action, peer, operation);
}
