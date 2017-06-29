
#ifndef _BSYNCPRIM_H_
#define _BSYNCPRIM_H_


#include "biu.h"

#if defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#include <semaphore.h>
#endif // __linux__


using namespace std;


namespace Biu {


	// Constant

#ifndef _WINDOWS
	const long INFINITE                             = -1;
#endif


	// Interface IBScopeLockable

	struct IBScopeLockable
	{
		virtual void ScopeLock() = 0;
		virtual void ScopeUnlock() = 0;
	};


	// Class BScopeLock

	class BIU_API BScopeLock
	{
	public:
		BScopeLock(IBScopeLockable* sl);
		~BScopeLock();
	private:
		IBScopeLockable* m_sl;
	};


	// Class BCriticalSection

	class BIU_API BCriticalSection : public IBScopeLockable
	{
	public:
		BCriticalSection();

#ifdef _WINDOWS
		BCriticalSection(DWORD dwSpinCount);
#endif

		~BCriticalSection();

		void Lock();
		void Unlock();
		bool TryLock();

	
		// IXScopeLockable

		virtual void ScopeLock();
		virtual void ScopeUnlock();

	private:

#ifdef _WINDOWS
		static const DWORD Default_Spin_Count;

		CRITICAL_SECTION m_cs;
#else
		pthread_mutex_t m_cs;
#endif // _WINDOWS
	};


	// Class BEvent

	class BIU_API BEvent
	{
	public:
		BEvent(bool bState = false);
		~BEvent();
	
		void Set();
		void Reset();
		bool Wait(long lMillisenonds = INFINITE);
		bool TryWait();
	
		bool IsSet();
	
	protected:
#ifdef _WINDOWS
		HANDLE m_hevt;
#else
		pthread_cond_t m_evt;
		pthread_mutex_t m_cs;
		bool m_bIsLocked;
#endif // _WINDOWS
	};


	// Class BSemaphore

	class BIU_API BSemaphore
	{
	public:
#ifdef _WINDOWS
		BSemaphore(long lCount, long lMaxCount);
#else
		BSemaphore(unsigned int nInitCount = 0, bool bShared = false);
#endif // _WINDOWS
		~BSemaphore();
	
		void Post(long lCount = 1);
		bool Wait(long lMilliseconds = INFINITE);
		bool TryWait();
		void Clear();
	
	protected:
#ifdef _WINDOWS
		HANDLE m_hsem;
		long m_lCount;
		long m_lMaxCount;
#else
		sem_t m_sem;
		unsigned int m_nInitCount;
		bool m_bShared;
#endif // _WINDOWS
	};

		
		// Class BReadWriteLock
		
		class BIU_API BReadWriteLock
		{
		public:
			BReadWriteLock();
			~BReadWriteLock();
			
			void LockRead();
			void LockWrite();
			bool TryLockRead();
			bool TryLockWrite();
			

#ifdef _WINDOWS
			void UnlockRead();
			void UnlockWrite();
#else
			void Unlock();

			bool TimedLockRead(long lMillisenonds = INFINITE);
			bool TimedLockWrite(long lMillisenonds = INFINITE);
#endif // _WINDOWS			
			
		private:
#ifdef _WINDOWS
			SRWLOCK m_h;
#else
			pthread_rwlock_t m_h;
#endif
		};


}

#endif // _BSYNCPRIM_H_
