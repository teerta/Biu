

#include "stdafx.h"

#ifdef _WINDOWS
#include "bmarco.h"
#else
#include <sys/time.h>
#endif // _WINDOWS

#include "bsyncprim.h"



using namespace Biu;


// BEvent

BEvent::BEvent(bool bState)
#ifdef _WINDOWS
	: m_hevt(NULL)
#else
	: m_bIsLocked(bState)
#endif // _WINDOWS
{
#ifdef _WINDOWS
	m_hevt = CreateEvent(NULL, TRUE, bState, NULL);
	if (!m_hevt)
	{
		throw GetLastError();
	}
#else
	int nRet = pthread_cond_init(&m_evt, NULL);    
	if (nRet)
	{
		throw nRet;
	}
	
	nRet = pthread_mutex_init(&m_cs, NULL);
	if (nRet)
	{
		throw nRet;
	}
#endif // _WINDOWS
}

BEvent::~BEvent()
{
#ifdef _WINDOWS
	SAFE_CLOSE_EVENT(m_hevt)
#else
	pthread_cond_destroy(&m_evt);
	pthread_mutex_destroy(&m_cs);
#endif // _WINDOWS
}

void BEvent::Set()
{
#ifdef _WINDOWS
	SetEvent(m_hevt);
#else
	pthread_mutex_lock(&m_cs);
	
	m_bIsLocked = false;
	pthread_cond_broadcast(&m_evt);
	
	pthread_mutex_unlock(&m_cs);
#endif // _WINDOWS
}

void BEvent::Reset()
{
#ifdef _WINDOWS
	ResetEvent(m_hevt);
#else
	pthread_mutex_lock(&m_cs);
	
	m_bIsLocked = true;
	
	pthread_mutex_unlock(&m_cs);
#endif // _WINDOWS
}

bool BEvent::Wait(long lMillisenonds)
{
	bool bRet = false;
	
#ifdef _WINDOWS
	bRet = WAIT_OBJECT_0 == WaitForSingleObject(m_hevt, (DWORD)lMillisenonds);
#else
	pthread_mutex_lock(&m_cs);
	
	if (m_bIsLocked)
	{
		if (INFINITE == lMillisenonds)
		{
			while (m_bIsLocked)
			{
				if (!pthread_cond_wait(&m_evt, &m_cs))
				{
					bRet = true;
				}
			}
		}
		else
		{
			timeval tv;
			timespec ts;
			gettimeofday(&tv, NULL);
			ts.tv_sec = ts.tv_sec + lMillisenonds / 1000;
			ts.tv_nsec = ts.tv_nsec + (lMillisenonds % 1000) * 1000000;
			
			if (!pthread_cond_timedwait(&m_evt, &m_cs, &ts))
			{
				bRet = true;
			}
		}
	}
	else
	{
		bRet = true;
	}
		
	pthread_mutex_unlock(&m_cs);
#endif // _WINDOWS  
	
	return bRet;
}

bool BEvent::TryWait()
{
	bool bRet = false;
 
#ifdef _WINDOWS
	bRet = WAIT_OBJECT_0 == WaitForSingleObject(m_hevt, 0);
#else
	pthread_mutex_lock(&m_cs);
	
	if (m_bIsLocked)
	{
		timeval tv;
		timespec ts;
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec;
		ts.tv_nsec = 0;
		
		if (!pthread_cond_timedwait(&m_evt, &m_cs, &ts))
		{
			bRet = true;
		}
	}
	else
	{
		bRet = true;
	}
	
	pthread_mutex_unlock(&m_cs);
#endif // _WINDOWS

	return bRet;
}

bool BEvent::IsSet()
{
	bool bRet = false;
	
#ifdef _WINDOWS
	bRet = WAIT_OBJECT_0 == WaitForSingleObject(m_hevt, 0);
#else
	pthread_mutex_lock(&m_cs);
	bRet = !m_bIsLocked;
	pthread_mutex_unlock(&m_cs);
#endif // _WINDOWS
	
	return bRet;
}
