
#include "stdafx.h"
#include "priv/i_nio.h"
#include "priv/i_peer.h"
#include "priv/bioctxt.h"


using namespace Biu;


// BNetExecutionUnit

//void BNetIOExecutionUnit::ExecuteIO(LPOVERLAPPED_ENTRY poe, BOOL bSucc)
void I_NetIOExecutionUnit::ExecuteIO(DWORD cb, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped, BOOL bSucc)
{
	DWORD operation = 0;
	BPeer* peer = NULL;
	I_Peer* pIntern = NULL;
	//BIOContext* pContext = reinterpret_cast<BIOContext*>(poe->lpOverlapped);
	BIOContext* pContext = reinterpret_cast<BIOContext*>(lpOverlapped);
	if (pContext)
	{
		operation = pContext->m_operation;
		//peer = static_cast<BPeer*>(pContext->m_pDisp);
		//peer = reinterpret_cast<BPeer*>(poe->lpCompletionKey);
		peer = reinterpret_cast<BPeer*>(lpCompletionKey);
		pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
		if (!bSucc && (BIO_Accept & operation))
		{
			BServerPeer* pServer = pIntern->m_pServer;
			if (peer->IsServer() && pServer->IsRunning())
			{
				BPeer* pAccept = static_cast<BPeer*>(pContext->m_pDisp);
				pAccept->DecRefIO();
				//peer->GetServer()->DecAccept();
				//pAccept->Destroy(true);
				pServer->FinishPending(pAccept, false);
				pIntern->NotifyNetNameUnavailable();

				PostAccept(peer);
			}

			return;
		}
	}
	else
	{
		// handle error here
		return;
	}

	if (bSucc)
	{		
		if (BIO_Accept & operation)
		{
			BPeer* pAccept = static_cast<BPeer*>(pContext->m_pDisp);
			pIntern = reinterpret_cast<I_Peer*>(pAccept->GetIntern());
			pIntern->EndAccept(peer);
		}
		else if (BIO_Connect & operation)
		{
			pIntern->EndConnect();
		}
		else if (BIO_Disconnect & operation)
		{
			pIntern->EndDisconnect();
		}
		else if (BIO_Read & operation)
		{
			//peer->EndRead(poe->dwNumberOfBytesTransferred);
			pIntern->EndRead(cb);
		}
		else if (BIO_Write & operation)
		{
			//peer->EndWrite(poe->dwNumberOfBytesTransferred);
			pIntern->EndWrite(cb);
		}
		else if (BIO_File & operation)
		{
			//peer->EndTransmitFile(poe->dwNumberOfBytesTransferred);
			pIntern->EndTransmitFile(cb);
		}
	}
	else
	{
		int nErr = WSAGetLastError();
		switch (nErr)
		{
		case WSAETIMEDOUT:
		case ERROR_SEM_TIMEOUT:
			peer->DecRefIO();
			pIntern->NotifyTimeout();
			if (BIO_Connect & operation)
			{				
				pIntern->Destroy();
			}			
			
			break;

		case ERROR_OPERATION_ABORTED:
			break;

		case ERROR_CONNECTION_REFUSED:
			pIntern->NotifyConnectRefused();
			break;

		case ERROR_NETNAME_DELETED:
			pIntern->DestroyUnavailability();
			break;

		default:
			pIntern->HandleError(BCS_NIoExecIo_ExecuteIo_UnhandledIoErr, nErr);
			break;
		}
	}
}
