
#include "stdafx.h"
#include "priv/bnex.h"
#include "priv/i_peer.h"
#include "bmarco.h"


using namespace Biu;


#ifdef _WINDOWS


// BSockExMethod

BSockExMethod::BSockExMethod()
{
}

BSockExMethod::~BSockExMethod()
{
}

BSockExMethod::BSockExMethod(BSockExMethod& xsem)
{
	Copy(xsem);
}

BSockExMethod& BSockExMethod::operator =(BSockExMethod& xsem)
{
	Copy(xsem);
	return *this;
}

void BSockExMethod::Copy(BSockExMethod& xsem)
{
	if (this != &xsem)
	{
		m_fnAcceptEx = xsem.m_fnAcceptEx;
		m_fnConnectEx = xsem.m_fnConnectEx;
		m_fnDisconnectEx = xsem.m_fnDisconnectEx;
		//m_fnTransmitFile = xsem.m_fnTransmitFile;
	}
}

bool BSockExMethod::Load(I_Peer* pPeerIntern)
{
	static GUID acceptex_guid = WSAID_ACCEPTEX;
	static GUID connectex_guid = WSAID_CONNECTEX;
	static GUID disconnectex_guid = WSAID_DISCONNECTEX;
	static GUID getacceptexsockaddrs_guid = WSAID_GETACCEPTEXSOCKADDRS;
	//static GUID transmitfile_guid = WSAID_TRANSMITFILE;

	CHK_false(pPeerIntern);

	SOCKET s = pPeerIntern->m_s;
	if (INVALID_SOCKET == s)
	{
		return false;
	}

	int nRet = 0;
	DWORD bytes = 0;

	nRet = WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &connectex_guid, sizeof(connectex_guid),
		&m_fnConnectEx, sizeof(m_fnConnectEx), &bytes, NULL, NULL);
	if(nRet == SOCKET_ERROR)
	{
		//SendErrorNotification(E_LoadWinsockEx, 0, WSAGetLastError());
		
		return false;
	}

	nRet = WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &disconnectex_guid, sizeof(disconnectex_guid),
		&m_fnDisconnectEx, sizeof(m_fnDisconnectEx), &bytes, NULL, NULL);
	if(nRet == SOCKET_ERROR)
	{
		//SendErrorNotification(E_LoadWinsockEx, 0, WSAGetLastError());
		
		return false;
	}

	nRet = WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptex_guid, sizeof(acceptex_guid),
		&m_fnAcceptEx, sizeof(m_fnAcceptEx), &bytes, NULL, NULL);
	if(nRet == SOCKET_ERROR)
	{
		//SendErrorNotification(E_LoadWinsockEx, 0, WSAGetLastError());

		return false;
	}

	nRet = WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &getacceptexsockaddrs_guid, sizeof(getacceptexsockaddrs_guid),
		&m_fnGetAccepeExSockAddrs, sizeof(m_fnGetAccepeExSockAddrs), &bytes, NULL, NULL);
	if(nRet == SOCKET_ERROR)
	{
		//SendErrorNotification(E_LoadWinsockEx, 0, WSAGetLastError());

		return false;
	}

	//nRet = WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &transmitfile_guid, sizeof(transmitfile_guid),
	//	&m_fnTransmitFile, sizeof(m_fnTransmitFile), &bytes, NULL, NULL);
	//if (nRet == SOCKET_ERROR)
	//{		
	//	return false;
	//}

	return true;
}


#endif // _WINDOWS
