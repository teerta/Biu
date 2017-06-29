
#include "stdafx.h"
#include "priv/i_peer.h"
#include "priv/bioctxt.h"
#include "priv/i_iodispatcher.h"
#include "priv/i_nio.h"
#include "bbase.h"
#include "bstctl.h"
#include "bpeersvr.h"
#include "bnio.h"
#include "basobj.h"
#include "biorule.h"
#include "bstdx.h"
#include <cassert>

#ifdef _WINDOWS
#include "priv/bnex.h"
#else
#include <unistd.h>
#include <fcntl.h>
#endif


using namespace Biu;


// I_Peer

I_Peer::I_Peer()
#ifdef _WINDOWS
	: m_s(INVALID_SOCKET)
#else
	: m_s(-1)
#endif
	, m_bServer(false)
	, m_nID(0)
	, m_nServerID(0)
	, m_nRefID(0)
	, m_sockType(BST_IPv4_TCP)
	, m_nSendFlags(0)
	, m_nRecvFlags(0)
	, m_pServer(NULL)
	, m_trait(BSTR_Normal)
	, m_traitVal(NULL)
	, m_closeFlag(BPCF_None)
#ifdef _WINDOWS
	, m_pEx(NULL)
#endif
{
#ifdef _WINDOWS
	m_pEx = new BSockExMethod();
#endif

	m_bCompatibleInitialization = false;
}

I_Peer::~I_Peer()
{
#ifdef _WINDOWS
	if (INVALID_SOCKET != m_s)
#else
	if (-1 != m_s)
#endif
	{
		Destroy(true, BPCF_Terminate);
	}

#ifdef _WINDOWS
	SAFE_DELETE(m_pEx);
#endif
}

#ifndef _WINDOWS
bool I_Peer::SetOptNonBlocking()
{
	int flags = fcntl(m_s, F_GETFL, 0);
	if (flags == -1)
	{
		return false;
	}

	flags |= O_NONBLOCK;
	int s = fcntl(m_s, F_SETFL, flags);
	if (s == -1)
	{
		return false;
	}

	return true;
}
#endif

bool I_Peer::SetOptReuse()
{
	int bVal = 1;
	int nRet = setsockopt(m_s, SOL_SOCKET, SO_REUSEADDR, (char*)&bVal, sizeof(int));
	return 0 == nRet;
}

void I_Peer::SetConnected()
{	
	sockaddr_in sa = { 0 };
	socklen_t cb = sizeof(sa);
	if (!getsockname(m_s, (sockaddr*)&sa, &cb))
	{
		m_representation.SetSockAddr((sockaddr*)&sa);
	}

	m_pWriteContext->m_operation = BIO_Write;
}


bool I_Peer::Create(B_Sock_Type sockType, BIOExecutionUnit* pExecUnit, BIOHandler* pHandler, IBDataChannel* pProtocol, B_IO_Operation operation)
{
	assert(pExecUnit);
	assert(pHandler);
	assert(pProtocol);

	bool bRet = false;

#ifdef _WINDOWS
	if (INVALID_SOCKET == m_s)
#else
	if (-1 == m_s)
#endif
	{
		switch (sockType)
		{
		case BST_IPv4_TCP:
			bRet = CreateStreamSocket();
			break;
		case BST_IPv4_UDP:
			bRet = CreateDatagramSocket();
			break;
		case BST_IPv6_TCP:
			break;
		case BST_IPv6_UDP:
			break;
		case BST_Unix_Domain_TCP:
			bRet = CreateUnixDomainStreamSocket();
			break;
		case BST_Unix_Domain_UDP:
			bRet = CreateUnixDomainDatagramSocket();
			break;
		case BST_Raw:
			break;
		default:
			break;
		}

		CHK_false(bRet);
	}

	m_sockType = sockType;
	m_pExecUnit = pExecUnit;
	m_pHandler = pHandler;

	if (pProtocol)
	{
		if (m_pProtocol)
		{
			delete m_pProtocol;
		}
		m_pProtocol = pProtocol->Clone();
	}

	CHK_false(SetOptReuse());
#ifndef _WINDOWS
	CHK_false(SetOptNonBlocking());
#endif

	m_pReadContext->m_pDisp = m_pOutter;
	m_pReadContext->Clear();
	m_pReadContext->m_operation = BIO_Accept == operation ? BIO_Accept : BIO_Read;

	m_pWriteContext->m_pDisp = m_pOutter;
	m_pWriteContext->Clear();
	m_pWriteContext->m_operation = BIO_Connect == operation ? BIO_Connect : BIO_Write;

#ifdef _WINDOWS
	m_pEx->Load(this);
#endif

	return NotifyNewHandle();
}

#ifndef _WINDOWS
bool I_Peer::CreateFrom(int s, B_Sock_Type sockType, BIOExecutionUnit* pExecUnit)
{
	assert(-1 != s);
	assert(pExecUnit);

	m_sockType = sockType;
	m_pExecUnit = pExecUnit;
	m_s = s;

	if (pExecUnit->GetProtocol())
	{
		if (m_pProtocol)
		{
			delete m_pProtocol;
		}
		m_pProtocol = pExecUnit->GetProtocol()->Clone();
	}

	CHK_false(SetOptReuse());
	CHK_false(SetOptNonBlocking());

	m_pReadContext->m_pDisp = this;
	m_pReadContext->Clear();
	m_pReadContext->m_operation = BIO_Read;

	m_pWriteContext->m_pDisp = this;
	m_pWriteContext->Clear();
	m_pWriteContext->m_operation = BIO_Write;

	return NotifyNewHandle();
}
#endif

bool I_Peer::CreateAccept(BPeer* peer)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());

	if (Create(pIntern->m_sockType, pIntern->m_pExecUnit, pIntern->m_pHandler, pIntern->m_pProtocol, BIO_Accept))
	{
		m_llVar = pIntern->m_llVar;
		m_mode = pIntern->m_mode;
		
		m_trait = pIntern->m_trait;
		m_traitVal = pIntern->m_traitVal;

		m_bCompatibleInitialization = pIntern->m_bCompatibleInitialization.load();
		m_nServerID = pIntern->m_nServerID;
		m_nRefID = pIntern->m_nRefID;
		m_pServer = pIntern->m_pServer;
		m_nSendFlags = pIntern->m_nSendFlags;
		m_nRecvFlags = pIntern->m_nRecvFlags;

		m_pOutter->SetTimeout(pIntern->m_llTimeout);

		return true;
	}

	return false;
}

void I_Peer::Destroy(bool bNow, unsigned int flags)
{
	unsigned int ui = bNow ? BPCF_Close : BPCF_Retire;
	if (flags)
	{
		ui |= flags;
	}

	m_closeFlag = (B_Peer_Close_Flag)ui;

	if (bNow)
	{
		Close();
	}
	else
	{
		unsigned int uiRefIO = m_refIO;
		unsigned int uiRefReadIO = m_refReadIO;
		if (!uiRefIO || uiRefIO == uiRefReadIO)
		{
			Close();
		}
	}	
}

void I_Peer::DestroyUnavailability()
{
	//NotifyNetNameUnavailable();
	unsigned int flags = BPCF_None;
	if (!m_bServer && m_nServerID)
	{
		flags = BPCF_Reuse;
		//m_pServer->RemovePeer(this);
	}

	m_refIO = 0;

	Destroy(true, flags);
}

void I_Peer::Close()
{
#ifdef _WINDOWS
	if (BPCF_Reuse & m_closeFlag)
	{
		int nErr = BeginDisconnect();
		if (nErr)
		{
			HandleError(BCS_PeerI_Close_BeginDisconnect, nErr);
		}
	}
	else
	{
		if (BPCF_Abort & m_closeFlag)
		{
			Linger(0);
		}

		closesocket(m_s);
		m_s = INVALID_SOCKET;

		if (!(BPCF_Terminate & m_closeFlag))
		{
			NotifyCloseEx(true);
			m_pExecUnit->Reclaim(m_pOutter);
		}
		//if (!(BPCF_Terminate & m_closeFlag))
		//{
		//	m_pExecUnit->GetDispatcher()->PushReclaiming(this);
		//}
	}
#else
	close(m_s);
	m_s = -1;

	NotifyCloseEx(true);
	m_pExecUnit->GetDispatcher()->Reclaim(this);
#endif	
}

void I_Peer::Clear()
{	
	//m_s = -1;
	m_bServer = false;
	m_nID = 0;
	m_nServerID = 0;
	m_nRefID = 0;
	m_sockType = BST_IPv4_TCP;
	m_nSendFlags = 0;
	m_nRecvFlags = 0;
	m_pServer = NULL;
	m_trait = BSTR_Normal;
	m_traitVal = NULL;
	m_closeFlag = BPCF_None;
	m_bCompatibleInitialization = false;
}

void I_Peer::HandleError(BCallingSite callingSite, ssize_t err)
{
	switch (err)
	{
#ifdef _WINDOWS
	case WSAECONNREFUSED:
		//case ERROR_CONNECTION_REFUSED:
#else
	case ECONNREFUSED:
#endif
		NotifyConnectRefused();
		break;

#ifdef _WINDOWS
	case WSAEHOSTUNREACH:
	case WSAEHOSTDOWN:
		//case ERROR_HOST_UNREACHABLE:
		//case ERROR_HOST_DOWN:
#else
	case EHOSTUNREACH:
	case EHOSTDOWN:
#endif
		NotifyHostUnavailable();
		break;

#ifdef _WINDOWS
	case WSAENETDOWN:
#else
	case ENETDOWN:
#endif
		NotifyNetUnavailable();
		break;

#ifdef _WINDOWS
	case WSAECONNRESET:
#else
	case ECONNRESET:
#endif
		NotifyConnectionReset();
		break;

		// Windows: 
		/*
		Acceptex: The attempt to connect timed out without establishing a connection.
		*/
#ifdef _WINDOWS
	case WSAETIMEDOUT:
	case ERROR_SEM_TIMEOUT: // IOCP: ERROR_SEM_TIMEOUT is a replacement of WSAETIMEOUT ?
#else
	case ETIMEDOUT:
#endif
		NotifyTimeout();
		Destroy();
		break;

	default:
		NotifyError(callingSite, err);
		break;
	}
}

void I_Peer::Linger(int nLinger)
{
	// force the subsequent closesocket to be abortative.
	linger  lingerStruct;

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = nLinger;

	setsockopt(m_s, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct));
}


#ifndef _WINDOWS
int I_Peer::Accept()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> sw(m_refIO, func);
	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	sockaddr_in sa;
	socklen_t cbSa = B_Size_Of_SockAddr_In;
	int s;
	int nRet = 0;
	while (1)
	{
		if (BPCF_None != m_closeFlag)
		{
			return 0;
		}

		s = accept(m_s, (sockaddr*)&sa, &cbSa);
		if (s == -1)
		{
			nRet = errno;
			break;
		}

		BPeer* peer = new BPeer();
		if (!peer->CreateFrom(s, m_sockType, m_pExecUnit))
		{
			SAFE_DELETE(peer);
			pDispatcher->JoinTheCircle(this);
			return -1;
		}
		peer->m_remoteRepresentation.SetSockAddr(reinterpret_cast<sockaddr*>(&sa));
		peer->m_representation = m_representation;
		peer->m_bCompatibleInitialization = m_bCompatibleInitialization.load();

		m_pServer->AddPeer(peer);
		peer->m_pServer = m_pServer;
		if (pDispatcher->JoinTheCircle(peer))
		{
			peer->NotifyAccepted();
		}
		else
		{
			pDispatcher->JoinTheCircle(this);
			return -1;
		}
	}

	if (BPCF_None == m_closeFlag)
	{
		pDispatcher->JoinTheCircle(this);
	}
	return nRet;
}
#endif

#ifndef _WINDOWS
int I_Peer::Connect(sockaddr_in* psa)
{
	assert(psa);

	m_remoteRepresentation.SetSockAddr((sockaddr*)psa);
	int nRet = connect(m_s, reinterpret_cast<sockaddr*>(psa), B_Size_Of_SockAddr_In);
	if (nRet)
	{
		return errno;
	}

	return 0;
}
#endif

#ifndef _WINDOWS
ssize_t I_Peer::Read()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	if (m_bReading)
	{
		return 0;
	}

	bool bHasData = false;
	{
		BAtomicBoolSwitch sw(m_bReading);

		char buff[B_MAX_BUFF_SIZE];
		BMemoryShack_char* pShack = pDispatcher->GetShack();
		while (1)
		{
			if (BPCF_None != m_closeFlag)
			{
				return 0;
			}

			ssize_t nRet = recv(m_s, buff, B_MAX_BUFF_SIZE, m_nRecvFlags);
			if (!nRet)
			{
				// peer has performed an orderly shutdown
				return -1;
			}
			else if (-1 == nRet)
			{
				int nRet = errno;
				if (EAGAIN == nRet)
				{
					break;
				}
				return nRet;
			}

			if (!m_bCompatibleInitialization)
			{
				m_bCompatibleInitialization = true;
				continue;
			}

			BMemoryBrick* pBrick = m_pReadContext->NewBrick();
			BWriteBrick(pBrick, pShack, buff, nRet);
			bHasData = true;
		}
	}

	if (BPCF_None == m_closeFlag)
	{
		if (bHasData)
		{
			if ((m_pServer && BIO_RM_Parallel == m_pServer->GetMode()) || BIO_RM_Parallel == m_mode)
			{
				pDispatcher->JoinTheCircle(this);
			}
			m_refIO++;
			pDispatcher->PushWork(m_pReadContext);
		}
		else
		{
			pDispatcher->JoinTheCircle(this);
		}
	}

	return 0;
}
#endif

#ifndef _WINDOWS
ssize_t I_Peer::Write()
{
	BAtomicBoolSwitch sw(m_bWriting);
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	ssize_t nRet = 0;
	ssize_t nTotal = 0;
	BMemoryBrick* pBrick = m_pWriteContext->PopBrick();
	while (pBrick)
	{
		if (BPCF_Close == m_closeFlag)
		{
			m_pWriteContext->PushHeadBrick(pBrick);
			return 0;
		}

		nRet = send(m_s, pBrick->GetMemory(), pBrick->GetDataSize(), m_nSendFlags);
		if (-1 == nRet)
		{
			m_pWriteContext->PushHeadBrick(pBrick);
			nRet = errno;
			if (EAGAIN == nRet)
			{
				if (BPCF_Close != m_closeFlag)
				{
					m_refIO++;
					if (!pDispatcher->JoinTheCircleForWriting(this))
					{
						m_refIO--;
						return -1;
					}
				}
			}

			return nRet;
		}

		nTotal += nRet;
		if (pBrick->IsEndOfSeries())
		{
			NotifyWriteDone(nTotal);
			nTotal = 0;
		}

		pDispatcher->GetShack()->Push(pBrick);
		pBrick = m_pWriteContext->PopBrick();
	}

	return 0;
}
#endif

#ifndef _WINDOWS
void I_Peer::WriteAgain()
{
	m_refIO--;
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> sw(m_refIO, func);
	if (BPCF_Close != m_closeFlag)
	{
		m_refIO++;
		m_pExecUnit->GetDispatcher()->PushWork(m_pWriteContext);
	}
}
#endif

void I_Peer::Craft()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> sw(m_refIO, func);
	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	if (BPCF_None != m_closeFlag)
	{
		return;
	}

#ifdef _WINDOWS
	int nErr = 0;
#endif
	bool bJoin = true;
	B_Data_Channel_Result result = m_pReadContext->ParseData();
	B_IO_Operation op = BIO_None;
	switch (result)
	{
	case B_DC_R_Succeeded:
	case B_DC_R_More:
	{
		BMemoryBrick* pNewBrick = m_pReadContext->AppeasePeerBrick();
		op = BIO_Read;
		if (m_pHandler->HandleProtocol(m_pOutter, &op))
		{
			switch (op)
			{
			case BIO_None:
			{
				// Clear all reamining data
				m_pReadContext->ClearBrick();
			}
				break;

			case BIO_Retire:
			{
				bJoin = false;
				Destroy(false);
			}
				break;

			case BIO_Close:
			{
				bJoin = false;
				Destroy();
			}
				break;

			case BIO_Read:
				// Continue reading
				break;

				//                    case BIO_Write:
				//                    {
				//                        bJoin = false;
				//                    }
				//                        break;

			case BIO_Continue:
			{
				I_IODispatcher* pDispatcherIntern = reinterpret_cast<I_IODispatcher*>(m_pExecUnit->GetDispatcher()->GetIntern());

#ifdef _WINDOWS
				BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(TlsGetValue(pDispatcherIntern->TSK_Return_Brick));
#else
				BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(pthread_getspecific(BIODispatcher::TSK_Return_Brick));
#endif
				if (pReturnBrick)
				{
#ifdef _WINDOWS
					TlsSetValue(pDispatcherIntern->TSK_Return_Brick, NULL);
#else
					pthread_setspecific(BIODispatcher::TSK_Return_Brick, NULL);
#endif
					if (BPCF_None == m_closeFlag)
					{
						m_pWriteContext->PushBrick(pReturnBrick);
#ifdef _WINDOWS
						nErr = BeginWrite();
						if (nErr)
						{
							HandleError(BCS_PeerI_Craft_BeginWrite, nErr);
						}
#else
						m_refIO++;
						pDispatcher->PushWork(m_pWriteContext);
#endif
					}
					else
					{
						pDispatcher->GetShack()->Push(pReturnBrick);
					}
				}
			}
				break;
			}
		}
		else
		{
			// handle error
			// Clear all reamining data
			m_pReadContext->ClearBrick();
		}

		pDispatcher->GetShack()->Push(pNewBrick);
	}
		break;

	case B_DC_R_Failed:
	{
		// Clear all remaining data
		m_pReadContext->ClearBrick();
	}
		break;

	case B_DC_R_Continue:
		// Read again when SerialMode
		break;
	}

	if (BPCF_None == m_closeFlag && bJoin && ((m_pServer && BIO_RM_Serial == m_pServer->GetMode()) || BIO_RM_Serial == m_mode))
	{
#ifdef _WINDOWS
		nErr = BeginRead();
		if (nErr)
		{
			HandleError(BCS_PeerI_Craft_BeginRead, nErr);
		}
#else
		pDispatcher->JoinTheCircle(this);
#endif
	}
}


bool I_Peer::ActAsServer(sockaddr_in* psa, int nBacklog)
{
	assert(psa);
	assert(nBacklog);

	int nRet = ::bind(m_s, reinterpret_cast<sockaddr*>(psa), B_Size_Of_SockAddr_In);
	if (SOCKET_ERROR == nRet)
	{
		return false;
	}

	nRet = listen(m_s, nBacklog);
	if (SOCKET_ERROR == nRet)
	{
		return false;
	}

	m_bServer = true;

	sockaddr_in sa;
	socklen_t cb = B_Size_Of_SockAddr_In;
	if (SOCKET_ERROR == getsockname(m_s, reinterpret_cast<sockaddr*>(&sa), &cb))
	{
		return false;
	}

	m_representation.SetSockAddr(reinterpret_cast<sockaddr*>(&sa));

	return true;
}

bool I_Peer::ActAsClient(sockaddr_in* psa)
{
	assert(psa);

	int nRet = ::bind(m_s, reinterpret_cast<sockaddr*>(psa), B_Size_Of_SockAddr_In);
	if (SOCKET_ERROR == nRet)
	{
		HandleError(BCS_PeerI_ActAsClient_Bing, nRet);
		return false;
	}

	return true;
}

#ifndef _WINDOWS
bool I_Peer::HandleCompatibleInitialization()
{
	static const size_t Size_Of_Buff = 32;

	bool bRet = true;
	if (!m_bCompatibleInitialization)
	{
		m_bCompatibleInitialization = true;
		char buff[Size_Of_Buff];
		bRet = -1 == send(m_s, buff, Size_Of_Buff, 0);
	}

	return bRet;
}
#endif

bool I_Peer::Shutdown(bool bRead, bool bWrite)
{
	int nHow = SD_BOTH;

	if (bRead && bWrite)
	{
		nHow = SD_BOTH;
	}
	else if (!bRead && !bWrite)
	{
		return false;
	}
	else
	{
		if (bRead)
		{
			nHow = SD_RECEIVE;
		}
		else if (bWrite)
		{
			nHow = SD_SEND;
		}
	}

	return !shutdown(m_s, nHow);
}

#ifdef _WINDOWS
int I_Peer::BeginRead()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	BAtomicSimpleCountSwitch<atomic_uint> swReadCount(m_refReadIO);
	if (m_bReading || BPCF_None != m_closeFlag)
	{
		return 0;
	}

	/*
	@ 2012.2.29
	New: a socket can has 1 concurrent read now
	*/
	//if (BIO_Read & m_pReadContext->m_operation)
	//{
	//	return 0;
	//}

	m_pReadContext->m_operation = BIO_Read;

	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	int nErr = 0;
	int nRet = 0;
	{
		bool bReading = m_bReading;
		BAtomicBoolSwitch sw(m_bReading);
		if (m_bReading && bReading == m_bReading)
		{
			return 0;
		}

		//memset(&m_pReadContext->m_Overlapped, 0, sizeof(m_pReadContext->m_Overlapped));
		nRet = WSARecv(m_s, &m_pReadContext->m_wsaBuf, 1, NULL, (LPDWORD)&m_nRecvFlags, (LPOVERLAPPED)m_pReadContext, NULL);
	}

	nErr = WSAGetLastError();
	if (nRet == SOCKET_ERROR && (WSA_IO_PENDING != nErr))
	{
		// handle error here

		/*
		@ 2012.3.7
		Unmask reading if any error occured
		*/
		//m_pReadContext->m_operation = BIO_Error;
		//Destroy(false);
		nRet = nErr;
	}
	else
	{
		m_refIO++;
		m_refReadIO++;
		nRet = 0;
	}

	return nRet;
}
#endif

#ifdef _WINDOWS
void I_Peer::EndRead(DWORD cb)
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	BAtomicSimpleCountSwitch<atomic_uint> swReadCount(m_refReadIO);
	if (m_refIO)
	{
		m_refIO--;
	}

	if (m_refReadIO)
	{
		m_refReadIO--;
	}

	if (BPCF_None != m_closeFlag)
	{
		return;
	}

	if (cb)
	{
		BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
		BMemoryShack_char* pShack = pDispatcher->GetShack();
		{
			BAtomicBoolSwitch sw(m_bReading);
			//{
			//	BScopeLock sl(&m_csReadShack);
				BMemoryBrick* pBrick = m_pReadContext->NewBrick();
				BWriteBrick(pBrick, pShack, m_pReadContext->m_buffer, cb);
			//}			
		}

		if ((m_pServer && BIO_RM_Parallel == m_pServer->GetMode()) || BIO_RM_Parallel == m_mode)
		{
			int nErr = BeginRead();
			if (nErr)
			{
				HandleError(BCS_PeerI_EndRead_BeginRead, nErr);
			}
		}

		m_refIO++;
		pDispatcher->PushWork(m_pReadContext);
	}
	else
	{
		//Destroy(true, m_nServerID ? BPCF_Reuse : BPCF_None);

		//// The remote peer is closed
		//NotifyNetNameUnavailable();

		DestroyUnavailability();
	}
}
#endif

#ifdef _WINDOWS
int I_Peer::BeginWrite()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	if (m_bWriting || BPCF_Close & m_closeFlag)
	{
		return 0;
	}

	m_pWriteContext->m_operation = BIO_Write;

	bool bWriting = m_bWriting;
	BAtomicBoolSwitch sw(m_bWriting);
	if (m_bWriting && bWriting == m_bWriting)
	{
		return 0;
	}

	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	BMemoryBrick* pBrick = m_pWriteContext->PopBrick();
	if (!pBrick)
	{
		return 0;
	}

	if (BPCF_Close & m_closeFlag)
	{
		m_pWriteContext->PushHeadBrick(pBrick);
		return 0;
	}

	m_pWriteContext->m_wsaBuf.buf = pBrick->GetMemory();
	m_pWriteContext->m_wsaBuf.len = pBrick->GetDataSize();
	//memset(&m_pWriteContext->m_Overlapped, 0, sizeof(m_pWriteContext->m_Overlapped));
	int nRet = WSASend(m_s, &m_pWriteContext->m_wsaBuf, 1, NULL, m_nSendFlags, (LPOVERLAPPED)m_pWriteContext, NULL);
	int nErr = WSAGetLastError();
	if (nRet == SOCKET_ERROR && (WSA_IO_PENDING != nErr))
	{
		m_pWriteContext->PushHeadBrick(pBrick);
		//Destroy(false);
		nRet = nErr;
	}
	else
	{
		m_pWriteContext->m_pLastWriteBrick = pBrick;
		m_refIO++;
		nRet = 0;
	}

	return nRet;
}
#endif

#ifdef _WINDOWS
void I_Peer::EndWrite(DWORD cb)
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	if (m_refIO)
	{
		m_refIO--;
	}

	if (BPCF_Close & m_closeFlag)
	{
		return;
	}

	if (cb)
	{
		m_pWriteContext->m_nTotalWrite += cb;
		BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
		BMemoryBrick* pBrick = m_pWriteContext->m_pLastWriteBrick;
		m_pWriteContext->m_pLastWriteBrick = NULL;
		if (pBrick)
		{

			if (pBrick->IsEndOfSeries())
			{
				NotifyWriteDone(m_pWriteContext->m_nTotalWrite);
				m_pWriteContext->m_nTotalWrite = 0;
			}

			pDispatcher->GetShack()->Push(pBrick);
		}
		//if (m_pWriteContext->HasBricks())
		//{
		//	pDispatcher->PushWork(m_pWriteContext);
		//}
	}
	//else
	//{
	//	// write-buffer's problem ?
	//}

	if (m_pWriteContext->HasBricks())
	{
		int nErr = BeginWrite();
		if (nErr)
		{
			HandleError(BCS_PeerI_EndWrite_BeginWrite, nErr);
		}
	}
}
#endif

#ifdef _WINDOWS
int I_Peer::BeginAccept(BPeer* peer)
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);

	m_pReadContext->m_operation = BIO_Accept;

	memset(&m_pReadContext->m_Overlapped, 0, sizeof(m_pReadContext->m_Overlapped));
	int nRet = m_pEx->AcceptEx()(
		(SOCKET)peer->GetCircleHandle(),
		m_s,
		(LPVOID)m_pReadContext->m_buffer,
		B_Size_Of_AcceptEx_Available_Buffer,
		B_Size_Of_AcceptEx_Addr,
		B_Size_Of_AcceptEx_Addr,
		NULL,
		(LPOVERLAPPED)m_pReadContext
		);

	int nErr = WSAGetLastError();
	if (nRet == SOCKET_ERROR && (WSA_IO_PENDING != nErr))
	{
		// handle error here

		nRet = nErr;
	}
	else
	{
		m_refIO++;
		//SetTimeout(peer->GetTimeout());
		//m_pServer = peer->GetServer();
		//static_cast<BNetIOExecutionUnit*>(m_pExecUnit)->AddPendingAccept(this);
		peer->GetServer()->AddPendingAccept(static_cast<BPeer*>(m_pOutter));
		nRet = 0;
	}

	return nRet;
}
#endif

#ifdef _WINDOWS
void I_Peer::EndAccept(BPeer* pListen)
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	if (m_refIO)
	{
		m_refIO--;
	}

	if (BPCF_None != m_closeFlag)
	{
		return;
	}

	m_pServer = pListen->GetServer();
	//m_pServer->DecAccept();
	if (m_pServer->IsRunning())
	{
		if (!UpdateContext())
		{			
			HandleError(BCS_PeerI_EndAccept_UpdateContext, WSAGetLastError());
			return;
		}

		BNetIOExecutionUnit* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
		I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pServer->GetServerPeer()->GetIntern());
		LPSOCKADDR_IN psaLocal = NULL,
					  psaRemote = NULL;
		GetAcceptedPeerAddress(&psaLocal, &psaRemote);
		m_remoteRepresentation.SetSockAddr(reinterpret_cast<sockaddr*>(psaRemote));
		m_representation.SetSockAddr(reinterpret_cast<sockaddr*>(psaLocal));/* = *m_pServer->GetServerPeer()->GetRepresentation();*/
		m_bCompatibleInitialization = pIntern->m_bCompatibleInitialization.load();

		//pExecUnit->RemovePendingAccept(this);
		//m_pServer->AddPeer(this);
		m_pServer->FinishPending(static_cast<BPeer*>(m_pOutter));
		if (!pExecUnit->GetDispatcher()->JoinTheCircle(m_pOutter))
		{
			// WTF: (2015.5.16) ERROR_INVALID_PARAMETER is happened for CreateIoCompletionPort when windows (8.1 ?) updated some unknown patches.

			int nErr = WSAGetLastError();
			if (ERROR_INVALID_PARAMETER != nErr)
			{
				HandleError(BCS_PeerI_EndAccept_JoinTheCircle, nErr);
				return;
			}

			m_pOutter->SetInTheCircle();

			/*HandleError(WSAGetLastError());
			return;*/
		}

		I_NetIOExecutionUnit* pNIOIntern = reinterpret_cast<I_NetIOExecutionUnit*>(pExecUnit->GetIntern());
		if (NotifyAccepted())
		{
			//pExecUnit->GetDispatcher()->PushWork(m_pReadContext);
			int nErr = BeginRead();
			if (nErr)
			{
				HandleError(BCS_PeerI_EndAccept_BeginRead, nErr);
			}
		}
		else
		{
			pNIOIntern->CloseServerItem(pListen->GetID(), &m_remoteRepresentation);
		}
		
		pNIOIntern->PostAccept(pListen);
	}
}
#endif

#ifdef _WINDOWS
int I_Peer::BeginConnect(PSOCKADDR_IN psa)
{
	assert(psa);

	m_pWriteContext->m_operation = BIO_Connect;
	m_remoteRepresentation.SetSockAddr((PSOCKADDR)psa);

	memset(&m_pWriteContext->m_Overlapped, 0, sizeof(m_pWriteContext->m_Overlapped));
	DWORD cbSent;
	int nRet = m_pEx->ConnextEx()(
		m_s,
		(PSOCKADDR)psa,
		B_Size_Of_SockAddr_In,
		m_pWriteContext->m_buffer,
		32,
		&cbSent,
		(LPOVERLAPPED)m_pWriteContext
		);

	int nErr = WSAGetLastError();
	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != nErr))
	{
		nRet = nErr;
	}
	else
	{
		nRet = 0;
		m_refIO++;
	}

	return nRet;
}
#endif

#ifdef _WINDOWS
void I_Peer::EndConnect()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	if (m_refIO)
	{
		m_refIO--;
	}

	if (BPCF_None != m_closeFlag)
	{
		return;
	}

	if (!UpdateContext())
	{
		HandleError(BCS_PeerI_EndConnect_UpdateContext, WSAGetLastError());
		return;
	}

	BNetIOExecutionUnit* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	SOCKADDR_IN saLocal;
	GetConnectedPeerAddress(&saLocal);
	m_representation.SetSockAddr(reinterpret_cast<sockaddr*>(&saLocal));

	m_bPending = false;
	NotifyConnected();
	//pExecUnit->GetDispatcher()->PushWork(m_pWriteContext);
	int nErr = BeginRead();
	if (nErr)
	{
		HandleError(BCS_PeerI_EndConnect_BeginRead, nErr);
	}
}
#endif

#ifdef _WINDOWS
int I_Peer::BeginDisconnect()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);

	m_pWriteContext->m_operation = BIO_Disconnect;

	//memset(&m_pWriteContext->m_Overlapped, 0, sizeof(m_pWriteContext->m_Overlapped));
	int nRet = m_pEx->DisconnectEx()(
		m_s,
		(LPOVERLAPPED)m_pWriteContext,
		TF_REUSE_SOCKET,
		0
		);

	int nErr = WSAGetLastError();
	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != nErr))
	{
		nRet = nErr;
	}
	else
	{
		nRet = 0;
		m_refIO++;
	}

	return nRet;
}
#endif

#ifdef _WINDOWS
void I_Peer::EndDisconnect()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	if (m_refIO)
	{
		m_refIO--;
	}

	NotifyCloseEx(false);
	//m_pExecUnit->GetDispatcher()->PushReclaiming(this);
	m_pExecUnit->Reclaim(m_pOutter);
}
#endif

//#ifdef _WINDOWS
//int I_Peer::BeginSysTransmitFile()
//{
//	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
//	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
//
//	auto* pDI = reinterpret_cast<BDataInfo*>(m_traitVal);
//
//	m_pWriteContext->m_operation = BIO_File;
//	//memset(&m_pWriteContext->m_Overlapped, 0, sizeof(m_pWriteContext->m_Overlapped));
//	TRANSMIT_FILE_BUFFERS tfb = { 0 };
//	tfb.Head = m_pWriteContext->m_buffer;
//	tfb.HeadLength = m_pWriteContext->m_wsaBuf.len;
//	int nRet = m_pEx->TransmitFile()(
//		m_s,
//		pDI->GetHandle(),
//		1024,
//		0,
//		(LPOVERLAPPED)m_pWriteContext,
//		&tfb,
//		TF_USE_KERNEL_APC
//		);
//
//	int nErr = WSAGetLastError();
//	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != nErr))
//	{
//		nRet = nErr;
//	}
//	else
//	{
//		nRet = 0;
//		m_refIO++;
//	}
//
//	return nRet;
//
//
//	//auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
//	//BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
//
//	//m_pWriteContext->m_operation = BIO_File;
//	//TRANSMIT_FILE_BUFFERS tfb = { 0 };
//	//tfb.Head = m_pWriteContext->m_buffer;
//	//tfb.HeadLength = m_pWriteContext->m_wsaBuf.len;
//	//int nRet = m_pEx->TransmitFile()(
//	//	m_s,
//	//	m_pWriteContext->m_hFile,
//	//	0, 
//	//	0, 
//	//	(LPOVERLAPPED)m_pWriteContext,
//	//	&tfb, 
//	//	TF_REUSE_SOCKET | TF_USE_KERNEL_APC
//	//	);
//
//	//int nErr = WSAGetLastError();
//	//if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != nErr))
//	//{
//	//	nRet = nErr;
//	//}
//	//else
//	//{
//	//	nRet = 0;
//	//	m_refIO++;
//	//}
//
//	//return nRet;
//}
//#endif
//
//#ifdef _WINDOWS
//void I_Peer::EndSysTransmitFile(DWORD cb)
//{
//	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
//	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
//
//	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
//	auto* pDI = reinterpret_cast<BDataInfo*>(m_traitVal);
//
//	NotifyFile(pDI->GetID(), pDI);
//
//	bool bRet = false;
//	BWaitDataInfo* pWait = NULL;
//	BPeer* primaryPeer = NULL;
//	if (m_pServer)
//	{
//		// Accept
//
//		auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
//		CHK(pData);
//
//		bRet = pData->PostTransmit(pDI->GetID(), primaryPeer, pWait);
//	}
//	else
//	{
//		// Connect
//
//		auto* pData = pExecUnit->GetClientData(m_nID);
//		if (!pData)
//		{
//			pData = pExecUnit->GetClientData(m_nRefID);
//		}
//		CHK(pData);
//
//		bRet = pData->PostTransmit(pDI->GetID(), primaryPeer, pWait);
//	}
//
//	if (bRet)
//	{
//		NotifyFileNext(primaryPeer, pWait);
//	}
//
//	SAFE_DELETE(pWait);
//
//
//	//auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
//	//BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
//
//	//m_pWriteContext->CloseFile();
//	//NotifyFile(m_pWriteContext->m_fileResidentID, m_pWriteContext->m_fileID, m_pWriteContext->m_fileSize);
//}
//#endif

#ifdef _WINDOWS
int I_Peer::BeginTransmitFile()
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	if (m_bWriting || BPCF_Close & m_closeFlag)
	{
		return 0;
	}

	int nRet = 0;
	unsigned long long cb = 0ull;
	{
		BAtomicBoolSwitch sw(m_bWriting);

		BMemoryShack_char* pShack = m_pExecUnit->GetDispatcher()->GetShack();
		auto* pDI = reinterpret_cast<BDataInfo*>(m_traitVal);

		m_pWriteContext->m_operation = BIO_File;

		NotifyDataHeader(pDI);
		Biu_DataHeader dataHeader;
		
		dataHeader.m_Current = pDI->GetAvailable();
		dataHeader.m_BufferSize = B_MAX_BUFF_SIZE;
		BMemoryBrick* pBrick = pShack->Pop();
		if (!m_pProtocol->CreateObj(pBrick, &dataHeader, pDI->GetHeader()))
		{
			return -1;
		}

		size_t nPos = 0;
		CHK_false(m_pProtocol->GetReadFunc()(pBrick, m_pWriteContext->m_buffer, pBrick->UntilGetDataSize(), nPos));
		cb = pDI->Read(m_pWriteContext->m_buffer + nPos, B_MAX_BUFF_SIZE - nPos, false);
		pShack->Push(pBrick);
		m_pWriteContext->m_nLastBlockSize = nPos;
		m_pWriteContext->m_wsaBuf.len = nPos + (unsigned long)cb;
		m_pWriteContext->m_wsaBuf.buf = m_pWriteContext->m_buffer;		
		cb = m_pWriteContext->m_wsaBuf.len;		

		m_refIO++;

		//memset(&m_pWriteContext->m_Overlapped, 0, sizeof(m_pWriteContext->m_Overlapped));
		nRet = WSASend(m_s, &m_pWriteContext->m_wsaBuf, 1, NULL, m_nSendFlags, (LPOVERLAPPED)m_pWriteContext, NULL);
		if (nRet == SOCKET_ERROR)
		{
			nRet = 0;
			int nErr = WSAGetLastError();
			if (ERROR_IO_PENDING != nErr)
			{
				m_refIO--;
				nRet = nErr;
			}
		}
		else
		{
			nRet = 0;			
		}		
	}

	//int nErr = WSAGetLastError();
	//if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != nErr))
	//{
	//	nRet = nErr;
	//}
	//else
	//{
	//	nRet = 0;		
	//}

	return nRet;
}
#endif

#ifdef _WINDOWS
void I_Peer::EndTransmitFile(DWORD cb)
{
	auto func = [&]() { if (BPCF_None != m_closeFlag) Close(); };
	BAtomicCountSwitch<atomic_uint, decltype(func)> swCount(m_refIO, func);
	if (m_refIO)
	{
		m_refIO--;
	}

	if (cb <= 0 || BPCF_Close & m_closeFlag)
	{
		return;
	}

	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	auto* pDI = reinterpret_cast<BDataInfo*>(m_traitVal);

	{
		BAtomicBoolSwitch sw(m_bWriting);

		cb -= m_pWriteContext->m_nLastBlockSize;
		m_pWriteContext->m_nLastBlockSize = 0;
		pDI->IncFinished(cb);
	}
	
	NotifyFile(pDI->GetID(), pDI);

	if (pDI->IsFinished())
	{
		BWaitDataInfo* pWait = NULL;
		BPeer* primaryPeer = NULL;
		BDataContinuation continuation;
		if (m_pServer)
		{
			// Accept

			auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
			CHK(pData);

			pDI->CreateContinuation(continuation);
			pData->PostTransmit(pDI->GetID(), primaryPeer, pWait);
		}
		else
		{
			// Connect

			auto* pData = pExecUnit->GetClientData(m_nID);
			if (!pData)
			{
				pData = pExecUnit->GetClientData(m_nRefID);
			}
			CHK(pData);

			pDI->CreateContinuation(continuation);
			pData->PostTransmit(pDI->GetID(), primaryPeer, pWait);
		}

		NotifyFileNext(&continuation, pWait);

		SAFE_DELETE(pWait);
	}
	else
	{
		if (m_bWriting)
		{
			return;
		}

		m_pWriteContext->m_wsaBuf.len = 0;
		int nErr = BeginTransmitFile();
		if (nErr)
		{
			HandleError(BCS_PeerI_EndTransFile_BeginTransFile, nErr);
		}		
	}
}
#endif


#ifdef _WINDOWS
bool I_Peer::UpdateContext()
{
	int nRet = 0;
	DWORD dwVal = 1;
	if (m_nServerID)
	{
		HANDLE h = m_pServer->GetServerPeer()->GetCircleHandle();
		nRet = setsockopt(m_s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (PCHAR)&h, B_Size_Of_Socket);
	}
	else
	{
		nRet = setsockopt(m_s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
	}

	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}
#endif

#ifdef _WINDOWS
void I_Peer::GetAcceptedPeerAddress(PSOCKADDR_IN* ppsaLocal, PSOCKADDR_IN* ppsaRemote)
{
	int cbLocal = B_Size_Of_SockAddr_In;
	int cbRemote = B_Size_Of_SockAddr_In;;

	m_pEx->GetAcceptExSockAddrs()(
		(LPVOID)m_pReadContext->m_buffer,
		B_Size_Of_AcceptEx_Available_Buffer,
		B_Size_Of_AcceptEx_Addr,
		B_Size_Of_AcceptEx_Addr,
		(LPSOCKADDR*)ppsaLocal,
		&cbLocal,
		(LPSOCKADDR*)ppsaRemote,
		&cbRemote
		);
}
#endif

#ifdef _WINDOWS
void I_Peer::GetConnectedPeerAddress(PSOCKADDR_IN psaLocal)
{
	int cb = B_Size_Of_SockAddr_In;
	int nRet = getsockname(m_s, (PSOCKADDR)psaLocal, &cb);
	if (SOCKET_ERROR == nRet || !cb)
	{
		int nerr = WSAGetLastError();
		// handle error here
	}
}
#endif

//void I_Peer::RemoveFromCollection()
//{
//	if (m_pServer)
//	{
//		m_pServer->RemovePeer(GetRepresentation());
//	}
//	else
//	{
//		BNetIOExecutionUnit* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
//		if (m_nServerID)
//		{
//			pExecUnit->RemoveServer(m_nID);
//		}
//		else
//		{
//			pExecUnit->RemoveClient(m_nID);
//		}
//	}
//}

B_Peer_Transmit_Result I_Peer::Send(IBSerializable* pObj, void* p)
{
	assert(pObj);
	
	if (BPCF_None != m_closeFlag)
	{
		return BPTR_Closing;
	}

	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	BMemoryShack_char* pShack = pDispatcher->GetShack();
	BMemoryBrick* pBrick = pShack->Pop();
	B_Peer_Transmit_Result ret = BPTR_False;
	if (m_pProtocol->CreateObj(pBrick, pObj, p, m_pSecurityContext))
	{
		m_pWriteContext->PushBrick(pBrick);
#ifdef _WINDOWS
		int nErr = BeginWrite();
		if (nErr)
		{
			HandleError(BCS_PeerI_Send_BeginWrite, nErr);
		}
#else
		m_refIO++;
		pDispatcher->PushWork(m_pWriteContext);
#endif
		ret = BPTR_True;
	}
	else
	{
		pShack->Push(pBrick);
	}

	return ret;
}

#ifdef _WINDOWS
B_Peer_Transmit_Result I_Peer::OutterSend(void* pObj, void* p)
{
	assert(pObj);

	if (BPCF_None != m_closeFlag)
	{
		return BPTR_Closing;
	}

	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	BMemoryShack_char* pShack = pDispatcher->GetShack();
	BMemoryBrick* pBrick = pShack->Pop();
	B_Peer_Transmit_Result ret = BPTR_False;
	B_Sock_Outter_Create param{ pBrick, pObj, p };
	if (NotifyOutterCreate(&param))
	{
		m_pWriteContext->PushBrick(pBrick);
		int nErr = BeginWrite();
		if (nErr)
		{
			HandleError(BCS_PeerI_OutterSend_BeginWrite, nErr);
		}

		ret = BPTR_True;
	}
	else
	{
		pShack->Push(pBrick);
	}

	return ret;
}
#endif

B_Peer_Transmit_Result I_Peer::SendPod(char* pPod, size_t cbPod)
{
	assert(pPod);
	assert(cbPod);

	if (BPCF_None != m_closeFlag)
	{
		return BPTR_Closing;
	}

	BIODispatcher* pDispatcher = m_pExecUnit->GetDispatcher();
	BMemoryShack_char* pShack = pDispatcher->GetShack();
	BMemoryBrick* pBrick = pShack->Pop();
	m_pProtocol->CreatePod(pBrick, pPod, cbPod);
	m_pWriteContext->PushBrick(pBrick);
#ifdef _WINDOWS
	int nErr = BeginWrite();
	if (nErr)
	{
		HandleError(BCS_PeerI_SendPod_BeginWrite, nErr);
	}
#else
	m_refIO++;
	pDispatcher->PushWork(m_pWriteContext);
#endif

	return BPTR_True;
}

bool I_Peer::Return(IBSerializable* pObj, void* p)
{
	assert(pObj);

	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pExecUnit->GetDispatcher()->GetIntern());
	BMemoryShack_char* pShack = m_pExecUnit->GetDispatcher()->GetShack();
#ifdef _WINDOWS
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(TlsGetValue(pIntern->TSK_Return_Brick));
#else
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(pthread_getspecific(BIODispatcher::TSK_Return_Brick));
#endif
	BMemoryBrick* pBrick = pShack->Pop();
	bool bRet = false;
	if (m_pProtocol->CreateObj(pBrick, pObj, p, m_pSecurityContext))
	{
		if (pReturnBrick)
		{
			pReturnBrick->GetLastBrick()->SetNext(pBrick);
		}
		else
		{
			pReturnBrick = pBrick;
		}
		bRet = true;
	}
	else
	{
		pShack->Push(pBrick);
	}

#ifdef _WINDOWS
	TlsSetValue(pIntern->TSK_Return_Brick, pReturnBrick);
#else
	pthread_setspecific(BIODispatcher::TSK_Return_Brick, pReturnBrick);
#endif

	return bRet;
}

#ifdef _WINDOWS
bool I_Peer::OutterReturn(void* pObj, void* p)
{
	assert(pObj);

	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pExecUnit->GetDispatcher()->GetIntern());
	BMemoryShack_char* pShack = m_pExecUnit->GetDispatcher()->GetShack();
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(TlsGetValue(pIntern->TSK_Return_Brick));
	BMemoryBrick* pBrick = pShack->Pop();
	bool bRet = false;
	B_Sock_Outter_Create param{ pBrick, pObj, p };
	if (NotifyOutterCreate(&param))
	{
		if (pReturnBrick)
		{
			pReturnBrick->GetLastBrick()->SetNext(pBrick);
		}
		else
		{
			pReturnBrick = pBrick;
		}
		bRet = true;
	}
	else
	{
		pShack->Push(pBrick);
	}

	TlsSetValue(pIntern->TSK_Return_Brick, pReturnBrick);

	return bRet;
}
#endif

bool I_Peer::ReturnPod(char* pPod, size_t cbPod)
{
	assert(pPod);
	assert(cbPod);

	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pExecUnit->GetDispatcher()->GetIntern());
	BMemoryShack_char* pShack = m_pExecUnit->GetDispatcher()->GetShack();
#ifdef _WINDOWS
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(TlsGetValue(pIntern->TSK_Return_Brick));
#else
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(pthread_getspecific(BIODispatcher::TSK_Return_Brick));
#endif
	BMemoryBrick* pBrick = pShack->Pop();
	bool bRet = false;
	m_pProtocol->CreatePod(pBrick, pPod, cbPod);
	if (pReturnBrick)
	{
		pReturnBrick->GetLastBrick()->SetNext(pBrick);
	}
	else
	{
		pReturnBrick = pBrick;
	}
	bRet = true;

#ifdef _WINDOWS
	TlsSetValue(pIntern->TSK_Return_Brick, pReturnBrick);
#else
	pthread_setspecific(BIODispatcher::TSK_Return_Brick, pReturnBrick);
#endif

	return bRet;
}

BDataInfo* I_Peer::GetDataInfo(int nID)
{
	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BDataInfo* pDI = NULL;
	if (m_pServer)
	{
		// Accept

		auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
		CHK_false(pData);

		pDI = pData->GetDataInfo(nID);
		if (!pDI)
		{
			pDI = pData->GetDataInfo(BDT_RECVID(nID));
		}
	}
	else
	{
		// Connect

		auto* pData = pExecUnit->GetClientData(m_nID);
		if (!pData)
		{
			pData = pExecUnit->GetClientData(m_nRefID);
		}
		CHK_false(pData);

		pDI = pData->GetDataInfo(nID);
		if (!pDI)
		{
			pDI = pData->GetDataInfo(BDT_RECVID(nID));
		}
	}

	return pDI;
}

B_Peer_Transmit_Result I_Peer::InitSendFile(LPCTSTR lpszFile, void* pHeader, int& nID)
{
	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BDataInfo* pDI = NULL;
	bool bRet = false;
	if (m_pServer)
	{
		// Accept

		auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
		if (!pData)
		{
			return BPTR_False;
		}

		bool bRet = pData->Send(static_cast<BPeer*>(m_pOutter), lpszFile, pDI);
		if (bRet && !pDI)
		{
			return BPTR_Wait;
		}

		if (!bRet)
		{
			return BPTR_False;
		}
	}
	else
	{
		// Connect

		auto* pData = pExecUnit->GetClientData(m_nID);
		if (!pData)
		{
			return BPTR_False;
		}

		bool bRet = pData->Send(lpszFile, pDI);
		if (bRet && !pDI)
		{
			return BPTR_Wait;
		}

		if (!bRet)
		{
			return BPTR_False;
		}
	}

	pDI->SetHeader(pHeader);
	nID = pDI->GetID();

	return BPTR_True;
}

B_Peer_Transmit_Result I_Peer::InitRecvFile(int nID, unsigned long long cb, LPCTSTR lpszFile)
{
	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BDataInfo* pDI = NULL;
	bool bRet = false;
	if (m_pServer)
	{
		// Accept

		auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
		if (!pData)
		{
			return BPTR_False;
		}

		bool bRet = pData->Recv(static_cast<BPeer*>(m_pOutter), nID, cb, lpszFile, pDI);
		if (bRet && !pDI)
		{
			return BPTR_Wait;
		}

		if (!bRet)
		{
			return BPTR_False;
		}
	}
	else
	{
		// Connect

		auto* pData = pExecUnit->GetClientData(m_nID);
		if (!pData)
		{
			return BPTR_False;
		}

		bool bRet = pData->Recv(nID, cb, lpszFile, pDI);
		if (bRet && !pDI)
		{
			return BPTR_Wait;
		}

		if (!bRet)
		{
			return BPTR_False;
		}
	}

	nID = pDI->GetID();

	return BPTR_True;
}

bool I_Peer::ClientSendFile(int nID)
{
	if (m_pServer)
	{
		return false;
	}

	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BPeer* pSend = NULL;
	bool bRet = false;
	auto* pData = pExecUnit->GetClientData(m_nID);
	if (!pData)
	{
		pData = pExecUnit->GetClientData(m_nRefID);
	}

	CHK_false(pData);

	bRet = pData->PreTransmit(nID, pSend);
	CHK_false(bRet);

	auto* pDI = pData->Transmit(nID, pSend);
	CHK_false(pDI);

	I_Peer* pIntern = reinterpret_cast<I_Peer*>(pSend->GetIntern());
	int nErr = pIntern->BeginTransmitFile();
	if (nErr)
	{
		bRet = false;
		HandleError(BCS_PeerI_ClientSendFile_BeginTransFile, nErr);
	}
	else
	{
		bRet = true;
	}

	return bRet;
}

bool I_Peer::ClientSendFileFinal(int nID)
{
	if (m_pServer)
	{
		return false;
	}

	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BIODispatcher* pDispatcher = pExecUnit->GetDispatcher();
	BMemoryShack_char* pShack = pDispatcher->GetShack();
	BDataInfo* pDI = NULL;
	auto* pData = pExecUnit->GetClientData(m_nID);
	if (!pData)
	{
		pData = pExecUnit->GetClientData(m_nRefID);
	}

	CHK_false(pData);

	pDI = pData->Transmit(nID, static_cast<BPeer*>(m_pOutter));
	CHK_false(pDI);

	bool bRet = false;
	int nErr = BeginTransmitFile();
	if (nErr)
	{
		bRet = false;
		HandleError(BCS_PeerI_ClientSendFileFi_BeginTransFile, nErr);
	}
	else
	{
		bRet = true;
	}

	return bRet;
}

B_Peer_Transmit_Result I_Peer::ClientRecvFile(int nID, IBSerializable* pObj, void* p)
{
	if (m_pServer)
	{
		return BPTR_False;
	}

	nID = BDT_RECVID(nID);

	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BPeer* pSend = NULL;
	BDataInfo* pDI = NULL;
	bool bRet = false;
	auto* pData = pExecUnit->GetClientData(m_nID);
	if (!pData)
	{
		pData = pExecUnit->GetClientData(m_nRefID);
	}

	if (!pData)
	{
		return BPTR_False;
	}

	bRet = pData->PreTransmit(nID, pSend, &pDI);
	if (pDI->GetDirection())
	{
		return BPTR_False;
	}

	pDI->SetHeader(p);
	pDI->SetParameter(pObj);

	if (!bRet)
	{
		return BPTR_Pending;
	}

	pDI = pData->Transmit(nID, pSend);
	if (!pDI)
	{
		return BPTR_False;
	}

	return pSend->Send(pObj, p);
}

B_Peer_Transmit_Result I_Peer::ClientRecvFileFinal(int nID)
{
	if (m_pServer)
	{
		return BPTR_False;
	}

	nID = BDT_RECVID(nID);

	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	auto* pData = pExecUnit->GetClientData(m_nID);
	if (!pData)
	{
		pData = pExecUnit->GetClientData(m_nRefID);
	}

	if (!pData)
	{
		return BPTR_False;
	}

	auto* pDI = pData->Transmit(nID, static_cast<BPeer*>(m_pOutter));
	if (!pDI || pDI->GetDirection())
	{
		return BPTR_False;
	}

	return Send(reinterpret_cast<IBSerializable*>(pDI->GetParameter()), pDI->GetHeader());
}

bool I_Peer::ClientRecvFileData(int nID, BMemoryBrick* pBrick, size_t nPos)
{
	if (m_pServer)
	{
		return false;
	}

	nID = BDT_RECVID(nID);
	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	auto* pData = pExecUnit->GetClientData(m_nID);
	if (!pData)
	{
		pData = pExecUnit->GetClientData(m_nRefID);
	}

	CHK_false(pData);

	auto* pDI = pData->GetDataInfo(nID);
	if (!pDI || pDI->GetDirection())
	{
		return false;
	}

	while (pBrick)
	{
		pDI->Write(pBrick->GetOffsetMemory(nPos), pBrick->GetDataSize() - nPos);
		nPos = 0;
		pBrick = pBrick->GetNext();
	}

	NotifyFile(pDI->GetID(), pDI);
	if (pDI->IsFinished())
	{
		BWaitDataInfo* pWait = NULL;
		BPeer* primaryPeer = NULL;
		BDataContinuation continuation;
		pDI->CreateContinuation(continuation);
		pData->PostTransmit(pDI->GetID(), primaryPeer, pWait);
		NotifyFileNext(&continuation, pWait);
		SAFE_DELETE(pWait);
	}

	return true;
}

bool I_Peer::ServerRecvFile(int nID, BMemoryBrick* pBrick, size_t nPos)
{
	if (!m_pServer)
	{
		return false;
	}

	nID = BDT_RECVID(nID);

	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
	auto* pDI = pData->TransmitDirectly(nID, static_cast<BPeer*>(m_pOutter));
	if (!pDI || pDI->GetDirection())
	{
		return false;
	}

	while (pBrick)
	{
		pDI->Write(pBrick->GetOffsetMemory(nPos), pBrick->GetDataSize() - nPos);
		nPos = 0;
		pBrick = pBrick->GetNext();
	}

	NotifyFile(pDI->GetID(), pDI);
	if (pDI->IsFinished())
	{
		BWaitDataInfo* pWait = NULL;
		BPeer* primaryPeer = NULL;
		BDataContinuation continuation;
		pDI->CreateContinuation(continuation);
		pData->PostTransmit(pDI->GetID(), primaryPeer, pWait);
		NotifyFileNext(&continuation, pWait);
		SAFE_DELETE(pWait);
	}

	return true;
}

bool I_Peer::ServerSendFile(int nID)
{
	if (!m_pServer)
	{
		return false;
	}

	bool bRet = true;
	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
	auto* pDI = pData->TransmitDirectly(nID, static_cast<BPeer*>(m_pOutter));
	if (!pDI || !pDI->GetDirection())
	{
		return false;
	}

	int nErr = BeginTransmitFile();
	if (nErr)
	{
		bRet = false;
		HandleError(BCS_PeerI_ServerSendFile_BeginTransFile, nErr);
	}

	return bRet;
}

void I_Peer::TerminateTransmitting(int nID)
{
	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BDataInfo* pDI = NULL;
	bool bRet = false;
	if (m_pServer)
	{
		// Accept

		auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
		if (!pData)
		{
			return;
		}

		pData->TerminateTransmit(nID);
	}
	else
	{
		// Connect

		auto* pData = pExecUnit->GetClientData(m_nID);
		if (!pData)
		{
			return;
		}

		pData->TerminateTransmit(nID);
	}
}

void I_Peer::TerminateAllTransmittings(BPeerDataInfo** ppPDI)
{
	auto* pExecUnit = static_cast<BNetIOExecutionUnit*>(m_pExecUnit);
	BDataInfo* pDI = NULL;
	bool bRet = false;
	if (m_pServer)
	{
		// Accept

		auto* pData = pExecUnit->GetData(m_pServer->GetServerPeer()->GetID());
		if (!pData)
		{
			return;
		}

		auto* peer = static_cast<BPeer*>(m_pOutter);

		pData->TerminateTransmit(peer, ppPDI);
	}
}

#ifndef _WINDOWS
int I_Peer::GetOptError()
{
	int nError = 0;
	socklen_t cb = sizeof(nError);
	getsockopt(m_s, SOL_SOCKET, SO_ERROR, &nError, &cb);
	return nError;
}
#endif

void I_Peer::ConfigBehaviorArbiter()
{
	if (m_pServer)
	{
		return;
	}

	if (m_pBehaviorArbiter)
	{
		return;
	}

	m_pBehaviorArbiter = new BIOBehaviorArbiter();
}

BIOBehaviorArbiter* I_Peer::GetBehaviorArbiter()
{
	if (m_pServer)
	{
		return m_pServer->GetBehaviorArbiter();
	}

	return m_pBehaviorArbiter;
}

BMemoryBrick* I_Peer::MoveOutBrick()
{
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pExecUnit->GetDispatcher()->GetIntern());
	BMemoryShack_char* pShack = m_pExecUnit->GetDispatcher()->GetShack();
#ifdef _WINDOWS
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(TlsGetValue(pIntern->TSK_Return_Brick));
#else
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(pthread_getspecific(BIODispatcher::TSK_Return_Brick));
#endif

	if (pReturnBrick)
	{
#ifdef _WINDOWS
		TlsSetValue(pIntern->TSK_Return_Brick, NULL);
#else
		pthread_setspecific(BIODispatcher::TSK_Return_Brick, NULL);
#endif
	}

	return pReturnBrick;
}

void I_Peer::MoveInBrick(BMemoryBrick* pBrick)
{
	CHK(pBrick);

	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pExecUnit->GetDispatcher()->GetIntern());
	BMemoryShack_char* pShack = m_pExecUnit->GetDispatcher()->GetShack();
#ifdef _WINDOWS
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(TlsGetValue(pIntern->TSK_Return_Brick));
#else
	BMemoryBrick* pReturnBrick = static_cast<BMemoryBrick*>(pthread_getspecific(BIODispatcher::TSK_Return_Brick));
#endif

	if (pReturnBrick)
	{
		pReturnBrick->GetLastBrick()->SetNext(pBrick);
	}
	else
	{
		pReturnBrick = pBrick;
	}

#ifdef _WINDOWS
	TlsSetValue(pIntern->TSK_Return_Brick, pReturnBrick);
#else
	pthread_setspecific(BIODispatcher::TSK_Return_Brick, pReturnBrick);
#endif
}
