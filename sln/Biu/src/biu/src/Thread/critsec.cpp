
#include "stdafx.h"
#include "bsyncprim.h"


using namespace Biu;


// BCriticalSection

#ifdef _WINDOWS
const DWORD BCriticalSection::Default_Spin_Count = 4000;
#endif // _WINDOWS

BCriticalSection::BCriticalSection()
{
#ifdef _WINDOWS
	if (!InitializeCriticalSectionAndSpinCount(&m_cs, Default_Spin_Count))
	{
		throw GetLastError();
	}
#else
	pthread_mutex_init(&m_cs, NULL);
#endif // _WINDOWS
}

#ifdef _WINDOWS
BCriticalSection::BCriticalSection(DWORD dwSpinCount)
{
	InitializeCriticalSectionAndSpinCount(&m_cs, dwSpinCount ? dwSpinCount : Default_Spin_Count);
}
#endif // _WINDOWS

BCriticalSection::~BCriticalSection()
{
#ifdef _WINDOWS
	DeleteCriticalSection(&m_cs);
#else
	pthread_mutex_destroy(&m_cs);
#endif // _WINDOWS
}

void BCriticalSection::Lock()
{
#ifdef _WINDOWS
	EnterCriticalSection(&m_cs);
#else
	pthread_mutex_lock(&m_cs);
#endif // _WINDOWS
}

void BCriticalSection::Unlock()
{
#ifdef _WINDOWS
	LeaveCriticalSection(&m_cs);
#else
	pthread_mutex_unlock(&m_cs);
#endif // _WINDOWS
}

bool BCriticalSection::TryLock()
{
#ifdef _WINDOWS
	return (bool)TryEnterCriticalSection(&m_cs);
#else
	return 0 == pthread_mutex_trylock(&m_cs);
#endif // _WINDOWS
}

void BCriticalSection::ScopeLock()
{
	Lock();
}

void BCriticalSection::ScopeUnlock()
{
	Unlock();
}
