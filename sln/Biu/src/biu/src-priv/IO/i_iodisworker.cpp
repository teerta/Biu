
#include "stdafx.h"
#include "priv/i_iodispatcher.h"
#include "biodef.h"
#include "bsyncprim.h"
#include "priv/bioctxt.h"
#include <algorithm>

#ifndef _WINDOWS
#include <unistd.h>
#include <signal.h>
#include <bits/signum.h>
#endif

#ifdef _linux_
#include <sys/epoll.h>
#endif

#include <cassert>
#include <iostream>


using namespace Biu;
using namespace std;


#ifdef _WINDOWS
DWORD WINAPI I_IODispatcher::ThreadWorker(LPVOID lpParam)
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

	TlsSetValue(pThis->TSK_Return_Brick, NULL);

	//int nCount = 0;
	BIOContext* pContext = NULL;
	//auto listReclaim = make_shared<list<BIODispatchable*> >();
	while (WAIT_OBJECT_0 != WaitForSingleObjectEx(pThis->m_hevtDone, INFINITE, TRUE))
	{
		//nCount = 0;
		pContext = pThis->PopWork();
		while (pContext)
		{
			if (pContext)
			{
				//nCount++;
				pContext->Work();
			}

			pContext = pThis->PopWork();

			//if (!(nCount % 10))
			//{
			//	if (pThis->TryPopReclaimings(listReclaim))
			//	{
			//		if (!listReclaim->empty())
			//		{
			//			for_each(listReclaim->begin(), listReclaim->end(), [&](BIODispatchable* pDisp)
			//			{
			//				pDisp->Reclaim();
			//			});
			//			listReclaim->clear();
			//		}
			//	}
			//}
		}

		//if (pThis->TryPopReclaimings(listReclaim))
		//{
		//	if (!listReclaim->empty())
		//	{
		//		for_each(listReclaim->begin(), listReclaim->end(), [&](BIODispatchable* pDisp)
		//		{
		//			pDisp->Reclaim();
		//		});
		//		listReclaim->clear();
		//	}
		//}

		pThis->MakeWorkerIdle(GetCurrentThreadId());
	}

	//if (pThis->TryPopReclaimings(listReclaim))
	//{
	//	if (!listReclaim->empty())
	//	{
	//		for_each(listReclaim->begin(), listReclaim->end(), [&](BIODispatchable* pDisp)
	//		{
	//			pDisp->Reclaim();
	//		});
	//		listReclaim->clear();
	//	}
	//}

	CoUninitialize();

	pThis->m_sufWorker.Give();

	return 0;
}
#endif

#ifndef _WINDOWS
void* I_IODispatcher::ThreadWorker(void* pParam)
{
	I_IODispatcher* pThis = static_cast<I_IODispatcher*>(pParam);
	assert(pThis);
	
	int nErr = 0;
	sigset_t sig;
	sigemptyset(&sig);
	if ((nErr = pthread_sigmask(SIG_SETMASK, &sig, NULL)))
	{
		return reinterpret_cast<void*>(nErr);
	}
	
	pthread_setspecific(TSK_Return_Brick, NULL);
	
	//auto listReclaim = make_shared<list<BIODispatchable*> >();
	auto listPending = make_shared<list<BIOContext*> >();
	while (1)
	{
		if (!pThis->m_semWork.Wait())
		{
			continue;
		}
		
		if (pThis->m_bTerm)
		{
			break;
		}
				
		BIOContext* pContext = pThis->PopWork();
		while (pContext)
		{
			if (!pContext->Work(listPending))
			{
				continue;
			}
			
			pContext = pThis->PopWork();
		}
		
		if (!listPending->empty())
		{
			pThis->PushWork(listPending);
			listPending->clear();
		}
		
		//if (TryPopReclaimings(listReclaim))
		//{
		//	if (!listReclaim->empty())
		//	{
		//		for_each(listReclaim->begin(), listReclaim->end(), [&](BIODispatchable* pDisp)
		//		{
		//			pDisp->Reclaim();
		//		});
		//}
	}
	}
	
	pThis->m_sufWorker.Give();
	cout << "Thread Worker - Term" << endl;
	return 0;
}
#endif
