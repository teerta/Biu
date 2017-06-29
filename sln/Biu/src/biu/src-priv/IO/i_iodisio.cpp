
#include "stdafx.h"
#include "priv/i_iodispatcher.h"
#include "priv/bioctxt.h"
#include "bpeer.h"
#include <algorithm>
#include <cassert>

#ifndef _WINDOWS
#include <unistd.h>
#include <signal.h>
#include <bits/signum.h>
#include <sys/epoll.h>
#include <iostream>
#endif


using namespace Biu;
using namespace std;


#if defined(_WINDOWS)
#define IOCP_MAX_ENTRY			16
#elif defined(_linux_)
#define EPOLL_MAX_WAITS         64
#endif


#ifdef _WINDOWS
DWORD I_IODispatcher::ThreadIO(LPVOID lpParam)
{
	I_IODispatcher* pThis = static_cast<I_IODispatcher*>(lpParam);
	if (!pThis)
	{
		return -1;
	}

	if (FAILED(CoInitialize(NULL)))
	{
		return -1;
	}

	DWORD dwRet = 0;
	BOOL bSuccess = FALSE;
	BIOContext* pContext = NULL;
	//OVERLAPPED_ENTRY oe[IOCP_MAX_ENTRY];
	//ULONG cbEntry = 0;
	//bool bDone = false;
	DWORD cb;
	ULONG_PTR lpCompletionKey;
	LPOVERLAPPED lpOverlapped;
	while(1)
	{
		cb = 0;
		lpCompletionKey = NULL;
		lpOverlapped = NULL;
		bSuccess = GetQueuedCompletionStatus(pThis->m_hBackend, &cb, &lpCompletionKey, &lpOverlapped, INFINITE);
		if (!lpCompletionKey)
		{
			dwRet = 0;
			break;
		}

		pContext = reinterpret_cast<BIOContext*>(lpOverlapped);
		pContext->Io(cb, lpCompletionKey, lpOverlapped, bSuccess);



		//bSuccess = GetQueuedCompletionStatusEx(pThis->m_hBackend, oe, IOCP_MAX_ENTRY, &cbEntry, INFINITE, TRUE);
		//for (ULONG i = 0; i < cbEntry; i++)
		//{
		//	if (!oe[i].lpCompletionKey)
		//	{
		//		bDone = true;
		//		dwRet = 0;
		//		break;
		//	}

		//	pContext = reinterpret_cast<BIOContext*>(oe[i].lpOverlapped);
		//	pContext->Io(&oe[i], bSuccess);
		//}

		//if (bDone)
		//{
		//	break;
		//}
	}

//#ifdef _DEBUG
//	CString str;
//	str.Format(_T("[BIODispatcher] ThreadIO %ld / %d has done.\r\n"), (int)++pThis->m_dbgClosedIOs, (int)pThis->m_dbgMaxIOs);
//	OutputDebugString(str);
//#endif

	pThis->m_sufIO.Give();

	CoUninitialize();

	return dwRet;
}
#endif // _WINDOWS

#ifdef _linux_
void* I_IODispatcher::ThreadIO(void* pParam)
{
	I_IODispatcher* pThis = static_cast<I_IODispatcher*>(pParam);
	assert(pThis);    
	
//    sigset_t sig;
//    sigemptyset(&sig);
//    sigaddset(&sig, SIGUSR2);
//    
//    signal(SIGUSR2, SigEndThread);
	
	int nErr = 0;
	sigset_t sig;
	sigemptyset(&sig);
//    sigaddset(&sig, SIGUSR2);
	if ((nErr = pthread_sigmask(SIG_SETMASK, &sig, NULL)))
	{
		return reinterpret_cast<void*>(nErr);
	}
//    
//    signal(SIGUSR2, SigEndThread);
	
	bool bTerm = false;
	epoll_event events[EPOLL_MAX_WAITS];
	int nEvents = 0;
	while (1)
	{
		if (bTerm)
		{
			break;
		}
		
//        nEvents = epoll_pwait(pThis->m_hBackend, events, EPOLL_MAX_WAITS, -1, &sig);
		nEvents = epoll_wait(pThis->m_hBackend, events, EPOLL_MAX_WAITS, 5000);
		if (-1 == nEvents)
		{
			if (EINTR == errno)
			{
				continue;
			}
		}
		
		if (pThis->m_bTerm)
		{
			break;
		}
		
		pThis->HandlePending();
		
		for (int i = 0; i < nEvents; ++i)
		{            
			BPeer* peer = NULL;
			if (pThis->m_hPipeIoTerm[0] != events[i].data.fd)
			{
				peer = reinterpret_cast<BPeer*>(events[i].data.ptr);
			}
			
			if (EPOLLERR & events[i].events)
			{
				// error happeded on the associated file descriptor
				
				peer->m_bInTheCircle = false;
				
				int nRet = peer->GetOptError();
				if (nRet)
				{
					pThis->NotifyError(peer, nRet);
					peer->RemoveFromCollection();
					pThis->RemovePendingConnect(peer);
					peer->Destroy();
				}
			}
			else if (EPOLLHUP & events[i].events)
			{
				// hang up on the associated file descriptor
				
				peer->m_bInTheCircle = false;
				int nRet = peer->GetOptError();
				if (nRet)
				{
					pThis->NotifyError(peer, nRet);
					peer->RemoveFromCollection();
					pThis->RemovePendingConnect(peer);
					peer->Destroy();
				}
			}
			else if (EPOLLRDHUP & events[i].events)
			{
				// remote peer is closed (close or shutdown writing half))
				
				peer->m_bInTheCircle = false;
				
				peer->NotifyClose(false);
				peer->RemoveFromCollection();
				pThis->RemovePendingConnect(peer);
				peer->Destroy();
			}
			else if (EPOLLPRI & events[i].events)
			{
				// OOB, urgent data
				
				peer->m_bInTheCircle = false;
			}
			else if (EPOLLOUT & events[i].events)
			{
				peer->m_bInTheCircleWrite = false;
				BIOContext* pContext = peer->m_pWriteContext;
				if (BIO_Connect & pContext->m_operation)
				{
					// The connecting has finished
					
					pThis->RemovePendingConnect(peer);
					int nRet = peer->GetOptError();
					if (nRet)
					{
						pThis->NotifyError(peer, nRet);
						peer->RemoveFromCollection();
						peer->Destroy();
					}
					else
					{
						peer->SetConnected();
						if (pThis->JoinTheCircle(peer))
						{
							peer->HandleCompatibleInitialization();
							peer->NotifyConnected();
						}
						else
						{
							pThis->NotifyError(peer, nRet);
						}
					}
				}
				else if (BIO_Write & pContext->m_operation)
				{
					// The sending is available again, resend pending data
					
					peer->WriteAgain();
				}
			}
			else if (EPOLLIN & events[i].events)
			{
				peer->m_bInTheCircle = false;
				
				// Term
				
				if (pThis->m_hPipeIoTerm[0] == events[i].data.fd)
				{
					bTerm = true;
					break;
				}
				
				BIOContext* pContext = peer->m_pReadContext;
				
				// Incoming data, read until EAGAIN
				
				if (BIO_Accept & pContext->m_operation)
				{
					// Accept
					
					int nRet = peer->Accept();
					if (nRet)
					{
						if (EAGAIN == nRet)
						{
							continue;
						}
						
						pThis->NotifyError(peer, nRet);
					}
				}
				else if (BIO_Read & pContext->m_operation)
				{
					// Read
					
					size_t nRet = peer->Read();
					if (nRet)
					{
						if (EAGAIN == nRet)
						{
							continue;
						}
						else if (-1 == nRet)
						{
							peer->NotifyClose(false);
						}
						else
						{
							pThis->NotifyError(peer, nRet);                            
						}
						
						peer->RemoveFromCollection();
						peer->Destroy();
					}
				}
			}
		}
	}

	pThis->m_sufIO.Give();
	cout << "Thread IO - Term" << endl;
	return 0;
}
#endif // _linux_
