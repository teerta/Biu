
#ifndef _I_IODISPATCHER_H_
#define _I_IODISPATCHER_H_


#include "biu.h"
#include "bbase.h"
#include "bmshack.h"
#include "bstdcollutil.h"
#include "bsyncprimx.h"


namespace Biu {
	
	//Forwarding

	struct BIOContext;
	class BIOExecutionUnit;
	class BIODispatchable;


	// Struct I_IODispatcher

	struct I_IODispatcher
	{
		I_IODispatcher();
		~I_IODispatcher();


#ifdef _WINDOWS
		DWORD TSK_Return_Brick;
#else
		static pthread_key_t TSK_Return_Brick;
#endif

#ifdef _WINDOWS
		BCriticalSection m_csWorker;
		BCriticalSection m_csBusyWorker;
#else
		BSemaphore m_semWork;
#endif
		BCountedSufficer m_sufIO;
		BCountedSufficer m_sufWorker;

		BCriticalSection m_csWork;
		//BCriticalSection m_csReclaim;
		BCriticalSection m_csUnits;

		list<BIOExecutionUnit*> m_units;
#ifdef _WINDOWS
		HANDLE m_rgThreadIO[B_MAX_WORKER_THREAD];

		list<HANDLE> m_workers;
		list<HANDLE> m_idleWorkers;
		list<HANDLE> m_busyWorkers;
		map<DWORD, HANDLE> m_workerID;
#else
		pthread_t m_rgThreadIO[B_MAX_WORKER_THREAD];
		pthread_t m_rgThreadWorker[B_MAX_WORKER_THREAD];

		int m_hPipeIoTerm[2];
#endif
		list<BIOContext*> m_listWork;
		//list<BIODispatchable*> m_listReclaim;


#if defined(_WINDOWS)
		HANDLE m_hBackend;
		HANDLE m_hevtDone;
#elif defined (_linux_) || defined (__APPLE__)
		int m_hBackend;
#endif		

		unsigned int m_cbThreadIO;
		unsigned int m_cbThreadWorker;

		BMemoryShack_char* m_pShack;
		atomic_bool m_bTerm;


		//inline void PushReclaiming(BIODispatchable* pDisp)
		//{
		//	BScopeLock sl(&m_csReclaim);
		//	m_listReclaim.push_back(pDisp);
		//}


		bool Create(BMemoryShack_char* pShack, unsigned int ulIOs = 0, unsigned int ulWorkers = 0);
		void Close();


#ifdef _WINDOWS
		static DWORD WINAPI ThreadIO(LPVOID lpParam);
		static DWORD WINAPI ThreadWorker(LPVOID lpParam);

		static void CALLBACK APCWakeup(ULONG_PTR dwParam);
#else
		static void* ThreadIO(void* pParam);
		static void* ThreadWorker(void* pParam);
#endif

		bool JoinTheCircle(BIODispatchable* pDisp);
#ifndef _WINDOWS
		bool JoinTheCircleForWriting(BIODispatchable* pDisp);
#endif

		void PushWork(BIOContext* pContext);
		BIOContext* PopWork();
#ifndef _WINDOWS
		void PushWork(shared_ptr<list<BIOContext*> >& list);
#endif

#ifdef _WINDOWS
		void DispatchWorker();
		void MakeWorkerIdle(DWORD dwThreadID);
#endif


		inline BMemoryShack_char* GetShack() { return m_pShack; }

		inline void AddUnit(BIOExecutionUnit* pUnit)
		{
			BSyncPushBack(m_csUnits, m_units, pUnit);
		}


		//inline bool TryPopReclaimings(shared_ptr<list<BIODispatchable*> >& list)
		//{
		//	if (!m_csReclaim.TryLock())
		//	{
		//		return false;
		//	}

		//	PopReclaimings(list);

		//	return true;
		//}

		//inline void PopReclaimings(shared_ptr<list<BIODispatchable*> >& list)
		//{
		//	BScopeLock sl(&m_csReclaim);
		//	copy(begin(m_listReclaim), end(m_listReclaim), list->begin());
		//	m_listReclaim.clear();
		//}

		//inline void DestroyReclaimings()
		//{
		//	BScopeLock sl(&m_csReclaim);
		//	for_each(begin(m_listReclaim), end(m_listReclaim), [&](BIODispatchable* pDisp)
		//	{
		//		delete pDisp;
		//	});
		//	m_listReclaim.clear();
		//}
	};


}


#endif // _I_IODISPATCHER_H_
