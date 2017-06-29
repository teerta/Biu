

#include "stdafx.h"

#ifdef _WINDOWS
#include "bmarco.h"
#else
#include <sys/time.h>
#endif // _WINDOWS

#include "bsyncprim.h"


using namespace Biu;


// BReadWriteLock

BReadWriteLock::BReadWriteLock()
{
#ifdef _WINDOWS
	InitializeSRWLock(&m_h);
#else
	pthread_rwlock_init(&m_h, NULL);
#endif
}

BReadWriteLock::~BReadWriteLock()
{
#ifdef _WINDOWS

#else
	pthread_rwlock_destroy(&m_h);
#endif
}

void BReadWriteLock::LockRead()
{
#ifdef _WINDOWS
	AcquireSRWLockShared(&m_h);
#else
	pthread_rwlock_rdlock(&m_h);
#endif
}

void BReadWriteLock::LockWrite()
{
#ifdef _WINDOWS
	AcquireSRWLockExclusive(&m_h);
#else
	pthread_rwlock_wrlock(&m_h);
#endif
}

bool BReadWriteLock::TryLockRead()
{
#ifdef _WINDOWS
	return TRUE == TryAcquireSRWLockShared(&m_h);
#else
	return !pthread_rwlock_tryrdlock(&m_h);
#endif
}

bool BReadWriteLock::TryLockWrite()
{
#ifdef _WINDOWS
	return TRUE == TryAcquireSRWLockExclusive(&m_h);
#else
	return !pthread_rwlock_trywrlock(&m_h);
#endif	
}

#ifdef _WINDOWS

void BReadWriteLock::UnlockRead()
{
	ReleaseSRWLockShared(&m_h);
}

void BReadWriteLock::UnlockWrite()
{
	ReleaseSRWLockExclusive(&m_h);
}

#else

void BReadWriteLock::Unlock()
{
	pthread_rwlock_unlock(&m_h);
}

bool BReadWriteLock::TimedLockRead(long lMillisenonds)
{
	timeval tv;
	timespec ts;
	gettimeofday(&tv, NULL);
	ts.tv_sec = ts.tv_sec + lMillisenonds / 1000;
	ts.tv_nsec = ts.tv_nsec + (lMillisenonds % 1000) * 1000000;
	
	return !pthread_rwlock_timedrdlock(&m_h, &ts);
}

bool BReadWriteLock::TimedLockWrite(long lMillisenonds)
{
	timeval tv;
	timespec ts;
	gettimeofday(&tv, NULL);
	ts.tv_sec = ts.tv_sec + lMillisenonds / 1000;
	ts.tv_nsec = ts.tv_nsec + (lMillisenonds % 1000) * 1000000;
	
	return !pthread_rwlock_timedwrlock(&m_h, &ts);
}
#endif // _WINDOWS
