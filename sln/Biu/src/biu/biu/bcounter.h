
#ifndef _BCOUNTER_H_
#define _BCOUNTER_H_


#include "biu.h"
#include <atomic>


using namespace std;


namespace Biu {


	// Class BSingleCounter

	class BIU_API BSingleCounter
	{
	public:
		BSingleCounter();
		BSingleCounter(unsigned long long ullVal, unsigned long long ullMax = 0ull);
		BSingleCounter(const BSingleCounter& rhs);
		virtual ~BSingleCounter();

		BSingleCounter& operator= (const BSingleCounter& rhs);


		inline const unsigned long long GetValue() { return atomic_load_explicit(&m_nVal, memory_order_relaxed); }
		inline void SetValue(const unsigned long long val) { atomic_store_explicit(&m_nVal, val, memory_order_relaxed); }

		inline const unsigned long long GetMax() { return m_nMax; }
		inline void SetMax(const unsigned long long val) { m_nMax = val; }

		inline void Clear()
		{
			atomic_store_explicit(&m_nVal, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_nMax, 0ull, memory_order_relaxed);
		}

		inline void Reset()
		{
			atomic_store_explicit(&m_nVal, 0ull, memory_order_relaxed);
		}

		bool CheckFailed(unsigned long long ullVal = 1ull, bool bReset = true);

	protected:
		atomic_uint64_t m_nVal;
		atomic_uint64_t m_nMax;

	private:
		void Copy(const BSingleCounter& rhs);
	};


	// Class BMultipleCounter

	class BIU_API BMultipleCounter
	{
	public:
		enum CheckResult : unsigned int
		{
			OK = 1,
			Failed,
			Warning,
			Banned
		};


		BMultipleCounter();
		BMultipleCounter(unsigned long long ullVal, unsigned long long ullBad = 5ull,
			unsigned long long ullWarning = 15ull, unsigned long long ullBan = 30ull, unsigned long long ullBannedTimeout = 600ull);
		BMultipleCounter(const BMultipleCounter& rhs);
		virtual ~BMultipleCounter();

		BMultipleCounter& operator=(const BMultipleCounter& rhs);


		inline const unsigned long long GetValue() { return atomic_load_explicit(&m_nVal, memory_order_relaxed); }
		inline void SetValue(const unsigned long long val) { atomic_store_explicit(&m_nVal, val, memory_order_relaxed); }

		inline const unsigned long long GetTotal() { return atomic_load_explicit(&m_nTotal, memory_order_relaxed); }
		inline void SetTotal(const unsigned long long val) { atomic_store_explicit(&m_nTotal, val, memory_order_relaxed); }

		inline const unsigned long long GetBad() { return atomic_load_explicit(&m_nBad, memory_order_relaxed); }
		inline void SetBad(const unsigned long long val) { atomic_store_explicit(&m_nBad, val, memory_order_relaxed); }

		inline const unsigned long long GetWarning() { return atomic_load_explicit(&m_nWarning, memory_order_relaxed); }
		inline void SetWarning(const unsigned long long val) { atomic_store_explicit(&m_nWarning, val, memory_order_relaxed); }

		inline const unsigned long long GetBan() { return atomic_load_explicit(&m_nBan, memory_order_relaxed); }
		inline void SetBan(const unsigned long long val) { atomic_store_explicit(&m_nBan, val, memory_order_relaxed); }

		inline bool IsLogged()
		{
			bool bLog = atomic_load_explicit(&m_bLog, memory_order_relaxed);
			bool bEnabled = atomic_load_explicit(&m_bEnabled, memory_order_relaxed);
			return bLog && bEnabled;
		}

		inline void SetLogged(bool val) { atomic_store_explicit(&m_bLog, val, memory_order_relaxed); }

		inline bool IsEnabled() { return atomic_load_explicit(&m_bEnabled, memory_order_relaxed); }
		inline void SetEnabled(bool val) { atomic_store_explicit(&m_bEnabled, val, memory_order_relaxed); }

		inline const unsigned long long GetBannedTimeout() { return atomic_load_explicit(&m_nBannedTimeout, memory_order_relaxed); }
		inline void SetBannedTimeout(const unsigned long long val) { atomic_store_explicit(&m_nBannedTimeout, val, memory_order_relaxed); }


		inline void Clear()
		{
			atomic_store_explicit(&m_nBannedTimeout, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_nVal, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_nTotal, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_nBad, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_nWarning, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_nBan, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_bLog, false, memory_order_relaxed);
			atomic_store_explicit(&m_bEnabled, true, memory_order_relaxed);
		}

		inline void Reset()
		{
			atomic_store_explicit(&m_nVal, 0ull, memory_order_relaxed);
		}

		inline void ResetTotal()
		{
			atomic_store_explicit(&m_nVal, 0ull, memory_order_relaxed);
			atomic_store_explicit(&m_nTotal, 0ull, memory_order_relaxed);
		}

		CheckResult Check(unsigned long long ullVal = 1ull, bool bReset = true);

	protected:
		atomic_uint64_t m_nVal;
		atomic_uint64_t m_nTotal;
		atomic_uint64_t m_nBad;
		atomic_uint64_t m_nWarning;
		atomic_uint64_t m_nBan;
		atomic_bool m_bLog;
		atomic_bool m_bEnabled;
		atomic_uint64_t m_nBannedTimeout;

	private:
		void Copy(const BMultipleCounter& rhs);
	};


}


#endif // _BCOUNTER_H_
