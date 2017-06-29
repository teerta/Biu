
#ifndef _BSTDX_H_
#define _BSTDX_H_


#include "biu.h"
#include <atomic>


using namespace std;


namespace Biu {


	// STL Atomic Helpers

	template<class T, memory_order mo = memory_order_relaxed>
	inline T SAtomicLoad(const atomic<T>& ato) noexcept
	{
		return atomic_load_explicit(&ato, mo);
	}

	template<memory_order mo = memory_order_relaxed>
	inline bool SAtomicLoad(const atomic_bool& ato) noexcept
	{
		return atomic_load_explicit(&ato, mo);
	}

	template<class T, memory_order mo = memory_order_relaxed>
	inline T SAtomicLoad(const volatile atomic<T>& ato) noexcept
	{
		return atomic_load_explicit(&ato, mo);
	}

	template<memory_order mo = memory_order_relaxed>
	inline bool SAtomicLoad(const volatile atomic_bool& ato) noexcept
	{
		return atomic_load_explicit(&ato, mo);
	}

	template<class T, memory_order mo = memory_order_relaxed>
	inline void SAtomicStore(atomic<T>& ato, T v) noexcept
	{
		atomic_store_explicit(&ato, v, mo);
	}

	template<memory_order mo = memory_order_relaxed>
	inline void SAtomicStore(atomic_bool& ato, bool v) noexcept
	{
		atomic_store_explicit(&ato, v, mo);
	}

	template<class T, memory_order mo = memory_order_relaxed>
	inline void SAtomicStore(volatile atomic<T>& ato, T v) noexcept
	{
		atomic_store_explicit(&ato, v, mo);
	}

	template<memory_order mo = memory_order_relaxed>
	inline void SAtomicStore(volatile atomic_bool& ato, bool v) noexcept
	{
		atomic_store_explicit(&ato, v, mo);
	}

	template<class T, memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(atomic<T>& atoDest, const atomic<T>& atoSrc) noexcept
	{
		auto v = atomic_load_explicit(&atoSrc, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}

	template<memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(atomic_bool& atoDest, const atomic_bool& atoSrc) noexcept
	{
		bool v = atomic_load_explicit(&atoSrc, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}

	template<class T, memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(atomic<T>& atoDest, const volatile atomic<T>& atoSrc) noexcept
	{
		auto v = atomic_load_explicit(&atoDest, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}

	template<memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(atomic_bool& atoDest, const volatile atomic_bool& atoSrc) noexcept
	{
		bool v = atomic_load_explicit(&atoDest, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}

	template<class T, memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(volatile atomic<T>& atoDest, const atomic<T>& atoSrc) noexcept
	{
		auto v = atomic_load_explicit(&atoSrc, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}

	template<memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(volatile atomic_bool& atoDest, const atomic_bool& atoSrc) noexcept
	{
		bool v = atomic_load_explicit(&atoSrc, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}

	template<class T, memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(volatile atomic<T>& atoDest, const volatile atomic<T>& atoSrc) noexcept
	{
		auto v = atomic_load_explicit(&atoDest, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}

	template<memory_order mol = memory_order_relaxed, memory_order mos = memory_order_relaxed>
	inline void SAtomicAssign(volatile atomic_bool& atoDest, const volatile atomic_bool& atoSrc) noexcept
	{
		bool v = atomic_load_explicit(&atoDest, mol);
		atomic_store_explicit(&atoDest, v, mos);
	}


}


#endif // _BSTDX_H_
