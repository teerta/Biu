
#ifndef _BSYNCPRIMX_H_
#define _BSYNCPRIMX_H_


#include "biu.h"
#include "bsyncprim.h"
#include <atomic>

#if defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#include <semaphore.h>
#endif // __linux__


using namespace std;


namespace Biu {


	// Class BCountedSufficer

	class BIU_API BCountedSufficer
	{
	public:
		BCountedSufficer(int nMax = 0, int nCurrent = 0);
		~BCountedSufficer();

		int IncMax();
		int IncMax(int nCount);
		int DecMax();
		void SetMax(int nMax);

		void Take();
		void Give();
		void Clear();

#ifdef _WINDOWS
		bool Wait(unsigned long ulMillisenonds = INFINITE);
		bool TryWait();

		const int GetMax();
		const int GetCurrent();
		bool IsEqual();
#else
		inline bool Wait(unsigned long ulMillisenonds = INFINITE)
		{
			return m_evt.Wait(ulMillisenonds);
		}

		inline bool TryWait()
		{
			return m_evt.TryWait();
		}

		inline const int GetMax() { return m_max; }
		inline const int GetCurrent() { return m_current; }
		inline bool IsEqual() { return m_max == m_current; }
#endif

	private:
		atomic<int32_t> m_max;
		atomic<int32_t> m_current;
		BEvent m_evt;

		void Suffice();
	};


	//// Class BUniSiSupplyChain
	///*
	//	Unidirectional-Single-Supply-Chain
	//*/

	//class BIU_API BUniSiSupplyChain
	//{
	//public:
	//	BUniSiSupplyChain();
	//	~BUniSiSupplyChain();

	//	inline void Start()
	//	{
	//		ResetEvent(m_hevtDone);
	//	}

	//	inline void Stop()
	//	{
	//		SetEvent(m_hevtDone);
	//	}

	//	inline bool Wait(DWORD dwMilliSeconds = INFINITE)
	//	{
	//		return WAIT_OBJECT_0 == WaitForSingleObjectEx(m_hevtDone, dwMilliSeconds, TRUE);
	//	}

	//	inline bool Wait(bool& bWorking, DWORD dwMilliSeconds = INFINITE)
	//	{
	//		DWORD dwWait = WaitForMultipleObjectsEx(2, m_waits, FALSE, dwMilliSeconds, TRUE);
	//		bWorking = WAIT_OBJECT_0 + 1 == dwWait;

	//		return WAIT_OBJECT_0 == dwWait;
	//	}

	//	inline void Provide()
	//	{
	//		SetEvent(m_hevtWorking);
	//	}
	//
	//	inline bool IsDone()
	//	{
	//		return WAIT_OBJECT_0 == WaitForSingleObject(m_hevtDone, 0);
	//	}

	//private:
	//	HANDLE m_hevtDone;
	//	HANDLE m_hevtWorking;
	//	HANDLE m_waits[2];
	//};


}


#endif // _BSYNCPRIMX_H_
