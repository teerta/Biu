
#include "stdafx.h"
#include "priv/i_iodispatcher.h"
#include "biodef.h"
#include "bbfunc.h"
#include "bsyncprim.h"
#include "priv/bioctxt.h"
#include "bstdcollutil.h"
#include "bbfunc.h"
#include <memory>
#include <algorithm>
#include <cassert>

#ifndef _WINDOWS
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <bits/signum.h>
#include <sys/epoll.h>
#endif


// I_IODispatcher

#ifndef _WINDOWS
pthread_key_t I_IODispatcher::TSK_Return_Brick;
#endif

I_IODispatcher::I_IODispatcher()
	: TSK_Return_Brick(0)
	, m_cbThreadIO(0)
	, m_cbThreadWorker(0)
	, m_hBackend(NULL)
	, m_pShack(NULL)
{
#ifdef _WINDOWS
	TSK_Return_Brick = TlsAlloc();
#else
	pthread_key_create(&TSK_Return_Brick, NULL);
#endif

	m_bTerm = false;
}

I_IODispatcher::~I_IODispatcher()
{
	Close();
#ifdef _WINDOWS
	TlsFree(TSK_Return_Brick);
#else
	pthread_key_delete(TSK_Return_Brick);
#endif
}

bool I_IODispatcher::Create(BMemoryShack_char* pShack, unsigned int ulIOs, unsigned int ulWorkers)
{
	assert(pShack);

	m_pShack = pShack;

#if defined(_WINDOWS)
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if (!ulIOs || ulIOs > B_MAX_WORKER_THREAD)
	{
		ulIOs = min<unsigned int>(si.dwNumberOfProcessors * 2, B_MAX_WORKER_THREAD);
	}

	if (!ulWorkers || ulWorkers > B_MAX_WORKER_THREAD)
	{
		ulWorkers = min<unsigned int>(si.dwNumberOfProcessors * 2, B_MAX_WORKER_THREAD);
	}

	m_cbThreadIO = ulIOs;
	m_cbThreadWorker = ulWorkers;

	ZeroMemory(m_rgThreadIO, B_MAX_WORKER_THREAD);

	m_hevtDone = CreateEvent(NULL, TRUE, FALSE, NULL);
	CHK_false(m_hevtDone);



	//m_hevtPeerManagementDone = CreateEvent(NULL, TRUE, FALSE, NULL);
	//CHK_false(m_hevtPeerManagementDone);

	m_hBackend = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CHK_false(m_hBackend);

	DWORD dwThreadID;
	//m_hThreadPeerManagement = CreateThread(NULL, 0, ThreadPeerManagement, this, 0, &dwThreadID);
	//CHK_false(m_hThreadPeerManagement);


	HANDLE  hThread;
	for (UINT32 i = 0; i < m_cbThreadIO; i++)
	{
		hThread = CreateThread(NULL, 0, ThreadIO, this, 0, &dwThreadID);
		if (!hThread)
		{
			CloseHandle(m_hBackend);
			m_hBackend = NULL;
			return false;
		}

		m_rgThreadIO[i] = hThread;
		hThread = INVALID_HANDLE_VALUE;
	}

	for (UINT32 i = 0; i < m_cbThreadWorker; i++)
	{
		hThread = CreateThread(NULL, 0, ThreadWorker, this, 0, &dwThreadID);
		if (!hThread)
		{
			CloseHandle(m_hBackend);
			m_hBackend = NULL;
			return false;
		}

		m_workers.push_back(hThread);
		m_idleWorkers.push_back(hThread);
		m_workerID[dwThreadID] = hThread;
	}
#elif defined(_linux_)
	if (-1 == (m_hBackend = epoll_create1(0)))
	{
		return false;
	}


	if (pipe(m_hPipeIoTerm))
	{
		return false;
	}

	int nFlags = fcntl(m_hPipeIoTerm[0], F_GETFL, 0);
	if (-1 == nFlags)
	{
		return false;
	}

	if (-1 == fcntl(m_hPipeIoTerm[0], F_SETFL, nFlags | O_NONBLOCK))
	{
		return false;
	}

	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = m_hPipeIoTerm[0];
	int nRet = epoll_ctl(m_hBackend, EPOLL_CTL_ADD, m_hPipeIoTerm[0], &event);
	if (-1 == nRet)
	{
		return false;
	}

	int nNumberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
	if (!ulIOs || ulIOs > B_MAX_WORKER_THREAD)
	{
		ulIOs = min<unsigned int>(nNumberOfProcessors * 2, B_MAX_WORKER_THREAD);
	}

	if (!ulWorkers || ulWorkers > B_MAX_WORKER_THREAD)
	{
		ulWorkers = min<unsigned int>(nNumberOfProcessors * 2, B_MAX_WORKER_THREAD);
	}

	m_cbThreadIO = ulIOs;
	m_cbThreadWorker = ulWorkers;

	memset(m_rgThreadIO, 0, B_MAX_WORKER_THREAD);
	memset(m_rgThreadWorker, 0, B_MAX_WORKER_THREAD);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_t pid;
	for (unsigned int i = 0; i < m_cbThreadIO; ++i)
	{
		if (pthread_create(&pid, &attr, ThreadIO, this))
		{
			return false;
		}
		m_rgThreadIO[i] = pid;
	}

	for (unsigned int i = 0; i < m_cbThreadWorker; ++i)
	{
		if (pthread_create(&pid, &attr, ThreadWorker, this))
		{
			return false;
		}
		m_rgThreadWorker[i] = pid;
	}

	pthread_attr_destroy(&attr);
#endif

	m_sufIO.SetMax(m_cbThreadIO);
	m_sufWorker.SetMax(m_cbThreadWorker);

	return true;
}

void I_IODispatcher::Close()
{
	CHK(m_hBackend);

#ifdef _WINDOWS
	SetEvent(m_hevtDone);

	m_sufWorker.Wait();

	//m_csTasks.Lock();

	//XIOContext* p = NULL;
	//while ((p = GetTask()))
	//{
	//	p->m_pp->m_lRefWorker = 0;
	//	p->m_pp->Close(false, false);
	//}

	//m_csTasks.Unlock();


	m_idleWorkers.clear();
	m_busyWorkers.clear();
	m_workerID.clear();

	{
		BScopeLock sl(&m_csWorker);
		DestructContainer(m_workers, [](HANDLE& handle) { CloseHandle(handle); });
	}

	CloseHandle(m_hevtDone);
	m_hevtDone = NULL;

	//m_sufIO.Wait();
	while (1)
	{
		for (UINT32 i = 0; i < m_cbThreadIO; i++)
		{
			PostQueuedCompletionStatus(m_hBackend, 0, 0, NULL);
		}

		if (m_sufIO.IsEqual())
		{
			break;
		}

		Sleep(500);
	}

	for (DWORD i = 0; i < m_cbThreadIO; i++)
	{
		if (m_rgThreadIO[i])
		{
			CloseHandle(m_rgThreadIO[i]);
			m_rgThreadIO[i] = NULL;
		}
	}

	CLOSE_HANDLE_NULL(m_hBackend);
#else

	// Term Worker

	m_bTerm = true;
	while (1)
	{
		m_semWork.Post(m_cbThreadWorker);
		if (m_sufWorker.IsEqual())
		{
			break;
		}
		else
		{
			usleep(500);
		}
	}

	// Term IO

	char data = 'z';
	while (1)
	{
		for (unsigned int i = 0; i < m_cbThreadIO; ++i)
		{
			write(m_hPipeIoTerm[1], &data, 1);
		}

		if (m_sufIO.IsEqual())
		{
			break;
		}
		else
		{
			usleep(500);
		}
	}

	{
		BScopeLock sl(&m_csServers);
		m_servers.ClearPointer();
	}

	{
		BScopeLock sl(&m_csClients);
		m_clients.ClearPointer();
	}

	close(m_hPipeIoTerm[0]);
	close(m_hPipeIoTerm[1]);

	close(m_hBackend);
#endif

	m_hBackend = NULL;

	DeleteContainer(m_units);
}

#ifdef _WINDOWS
void I_IODispatcher::APCWakeup(ULONG_PTR dwParam)
{
}
#endif

void I_IODispatcher::PushWork(BIOContext* pContext)
{
	{
		BScopeLock sl(&m_csWork);

		m_listWork.push_back(pContext);
	}

#ifdef _WINDOWS
	DispatchWorker();
#else
	if (!(BIO_Write == pContext->m_operation && pContext->m_pDisp->IsWriting()))
	{
		m_semWork.Post();
	}
#endif
}

#ifndef _WINDOWS
void I_IODispatcher::PushWork(shared_ptr<list<BIOContext*> >& list)
{
	{
		BScopeLock sl(&m_csWork);
		for_each(list->begin(), list->end(), [&](BIOContext* p)
		{
			m_listWork.push_back(p);
		});
	}

#ifdef _WINDOWS
	DispatchWorker();
#else
	m_semWork.Post();
#endif
}
#endif

BIOContext* I_IODispatcher::PopWork()
{
	BIOContext* pContext = NULL;
	{
		BScopeLock sl(&m_csWork);
		if (!m_listWork.empty())
		{
			pContext = m_listWork.front();
			m_listWork.pop_front();
		}
	}

	return pContext;
}

#ifdef _WINDOWS
void I_IODispatcher::DispatchWorker()
{
	HANDLE hWorker = NULL;
	{
		BScopeLock sl(&m_csWorker);
		if (m_idleWorkers.empty())
		{
			return;
		}

		hWorker = m_idleWorkers.front();
		m_idleWorkers.pop_front();
	}

	{
		BScopeLock sl(&m_csBusyWorker);
		m_busyWorkers.push_back(hWorker);
	}

	QueueUserAPC(APCWakeup, hWorker, NULL);
}
#endif

#ifdef _WINDOWS
void I_IODispatcher::MakeWorkerIdle(DWORD dwThreadID)
{
	HANDLE hWorker = NULL;
	{
		BScopeLock sl(&m_csWorker);
		hWorker = m_workerID[dwThreadID];
	}

	if (!hWorker)
	{
		return;
	}

	{
		BScopeLock sl(&m_csBusyWorker);
		m_busyWorkers.remove(hWorker);
	}

	{
		BScopeLock sl(&m_csWorker);
		m_idleWorkers.push_back(hWorker);
	}
}
#endif

bool I_IODispatcher::JoinTheCircle(BIODispatchable* pDisp)
{
	if (pDisp->IsInTheCircle())
	{
		return true;
	}

#if defined(_WINDOWS)
	if (CreateIoCompletionPort(pDisp->GetCircleHandle(), m_hBackend, (DWORD_PTR)pDisp, 0))
	{
		pDisp->SetInTheCircle();
	}
	else
	{
		// handle error here
		//SendErrorNotification(E_AddToIOCP, GetLastError(), 0);
		return false;
	}
#elif defined(_linux_)
	epoll_event event;
	event.events = EPOLLET | EPOLLONESHOT | EPOLLIN;
	event.data.ptr = pDisp;

	if (-1 == epoll_ctl(m_hBackend, EPOLL_CTL_ADD, pDisp->GetCircleHandle(), &event))
	{
		if (EEXIST == errno &&
			-1 == epoll_ctl(m_hBackend, EPOLL_CTL_MOD, pDisp->GetCircleHandle(), &event))
		{
			pDisp->SetInTheCircle(false);
			return false;
		}
	}
#endif

	return true;
}

#ifndef _WINDOWS
bool I_IODispatcher::JoinTheCircleForWriting(BIODispatchable* pDisp)
{
	if (pDisp->IsInTheCircleForWriting())
	{
		return true;
	}

	pDisp->SetInTheCircleForWriting();

	epoll_event event;
	event.events = EPOLLET | EPOLLONESHOT | EPOLLOUT;
	event.data.ptr = pDisp;

	if (-1 == epoll_ctl(m_hBackend, EPOLL_CTL_ADD, pDisp->GetCircleHandle(), &event))
	{
		if (EEXIST == errno &&
			-1 == epoll_ctl(m_hBackend, EPOLL_CTL_MOD, pDisp->GetCircleHandle(), &event))
		{
			pDisp->SetInTheCircleForWriting(false);
			return false;
		}
	}

	return true;
}
#endif
