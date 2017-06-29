
#include "stdafx.h"
#include "bsyncprim.h"

#ifdef _WINDOWS
#include "bmarco.h"
#endif


using namespace Biu;


// BSemaphore

#ifdef _WINDOWS
BSemaphore::BSemaphore(long lCount, long lMaxCount)
	: m_hsem(NULL)
	, m_lCount(lCount)
	, m_lMaxCount(lMaxCount)
{
	m_hsem = CreateSemaphore(NULL, lCount, lMaxCount, NULL);
	if (!m_hsem)
	{
		throw GetLastError();
	}
}
#else
BSemaphore::BSemaphore(unsigned int nInitCount, bool bShared)
		: m_nInitCount(nInitCount)
		, m_bShared(bShared)
{
	sem_init(&m_sem, bShared ? 1 : 0, nInitCount);
}
#endif // _WINDOWS

BSemaphore::~BSemaphore()
{
#ifdef _WINDOWS
	SAFE_CLOSE_HANDLE_NULL(m_hsem);
#else
	sem_destroy(&m_sem);
#endif // _WINDOWS
}

void BSemaphore::Post(long lCount)
{
#ifdef _WINDOWS
	ReleaseSemaphore(m_hsem, lCount, NULL);
#else
	for (int i = 0; i < lCount; ++i)
	{
		sem_post(&m_sem);
	}
#endif // _WINDOWS
}

bool BSemaphore::Wait(long lMilliseconds)
{
#ifdef _WINDOWS
	return WAIT_OBJECT_0 == WaitForSingleObject(m_hsem, (DWORD)lMilliseconds);
#else
	return 0 == sem_wait(&m_sem);
#endif // _WINDOWS
}

bool BSemaphore::TryWait()
{
#ifdef _WINDOWS
	return WAIT_OBJECT_0 == WaitForSingleObject(m_hsem, 0);
#else
	return 0 == sem_trywait(&m_sem);
#endif // WINDOWS
}

void BSemaphore::Clear()
{
#ifdef _WINDOWS
	SAFE_CLOSE_HANDLE_NULL(m_hsem);
	m_hsem = CreateSemaphore(NULL, m_lCount, m_lMaxCount, NULL);
	if (!m_hsem)
	{
		throw GetLastError();
	}
#else
	sem_destroy(&m_sem);
	sem_init(&m_sem, m_bShared ? 1 : 0, m_nInitCount);
#endif // _WINDOWS
}
