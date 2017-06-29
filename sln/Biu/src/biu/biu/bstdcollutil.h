
#ifndef _BSTDCOLLUTIL_H_
#define _BSTDCOLLUTIL_H_


#include "biu.h"
#include "bsyncprim.h"


namespace Biu {


	// for list, dequeue, vector and etc.

	template<
		class LOCK,
		template<class, class> class Collection,
		class T,
		class Allocator = std::allocator<T>
	>
	inline void BSyncPushBack(LOCK& lock, Collection<T, Allocator>& c, T& v)
	{
		BScopeLock sl(&lock);
		c.push_back(v);
	}

	template<
		class LOCK,
		template<class, class> class Collection,
		class T,
		class Allocator = std::allocator<T>
	>
	T BSyncPeekPointer(LOCK& lock, Collection<T, Allocator>& c)
	{
		T p = NULL;
		{
			BScopeLock sl(&lock);
			if (!c.empty())
			{
				p = c.front();
			}
		}

		return p;
	}

	template<
		class LOCK,
		template<class, class> class Collection,
		class T,
		class Allocator = std::allocator<T>
	>
	T BSyncPopPointer(LOCK& lock, Collection<T, Allocator>& c)
	{
		T p = NULL;
		{
			BScopeLock sl(&lock);
			if (!c.empty())
			{
				p = c.front();
				c.pop_front();
			}
		}

		return p;
	}

	template<
		class LOCK,
		template<class, class> class Collection,
		class T,
		class Allocator = std::allocator<T>
	>
	inline size_t BSyncGetSize(LOCK& lock, Collection<T, Allocator>& c)
	{
		Collection<T, Allocator>::size_type n = 0;
		{
			BScopeLock sl(&lock);
			n = c.size();
		}

		return n;
	}


	// for map, multimap and etc.

	template<
		class LOCK,
		template<class, class, class, class> class Collection,
		class K,
		class T,
		class Traits = less<K>,
		class Allocator = std::allocator<pair<const K, T> >
	>
	inline void BSyncAdd(LOCK& lock, Collection<K, T, Traits, Allocator>& c, K k, T& v)
	{
		BScopeLock sl(&lock);
		c[k] = v;
	}

	template<
		class LOCK,
		template<class, class, class, class> class Collection,
		class K,
		class T,
		class Traits = less<K>,
		class Allocator = std::allocator<pair<const K, T> >
	>
	T BSyncGetPointer(LOCK& lock, Collection<K, T, Traits, Allocator>& c, K k)
	{
		T p = NULL;
		{
			BScopeLock sl(&lock);
			auto iter = c.find(k);
			if (c.end() != iter)
			{
				p = iter->second;
			}
		}

		return p;
	}

	template<
		class LOCK,
		template<class, class, class, class> class Collection,
		class K,
		class T,
		class Traits = less<K>,
		class Allocator = std::allocator<pair<const K, T> >
	>
	bool BSyncGet(LOCK& lock, Collection<K, T, Traits, Allocator>& c, K k, T& v)
	{
		bool bRet = false;
		{
			BScopeLock sl(&lock);
			auto iter = c.find(k);
			if (c.end() != iter)
			{
				bRet = true;
				v = iter->second;
			}
		}

		return bRet;
	}

	template<
		class LOCK,
		template<class, class, class, class> class Collection,
		class K,
		class T,
		class Traits = less<K>,
		class Allocator = std::allocator<pair<const K, T> >
	>
	T BSyncGetPointer2(LOCK& lock, Collection<K, T, Traits, Allocator>& c, K k, bool bRemove = true)
	{
		T p = NULL;
		{
			BScopeLock sl(&lock);
			auto iter = c.find(k);
			if (c.end() != iter)
			{
				p = iter->second;
				if (bRemove)
				{
					c.erase(k);
				}
			}
		}

		return p;
	}

	template<
		class LOCK,
		template<class, class, class, class> class Collection,
		class K,
		class T,
		class Traits = less<K>,
		class Allocator = std::allocator<pair<const K, T> >
	>
	bool BSyncGet2(LOCK& lock, Collection<K, T, Traits, Allocator>& c, K k, T& v, bool bRemove = true)
	{
		bool bRet = false;
		{
			BScopeLock sl(&lock);
			auto iter = c.find(k);
			if (c.end() != iter)
			{
				bRet = true;
				v = iter->second;
				if (bRemove)
				{
					c.erase(k);
				}
			}
		}

		return bRet;
	}

	template<
		class LOCK,
		template<class, class, class, class> class Collection,
		class K,
		class T,
		class Traits = less<K>,
		class Allocator = std::allocator<pair<const K, T> >
	>
	T* BSyncEnsurePointer(LOCK& lock, Collection<K, T*, Traits, Allocator>& c, K k)
	{
		T* p = NULL;
		{
			BScopeLock sl(&lock);
			auto iter = c.find(k);
			if (c.end() == iter)
			{
				p = new T();
				c[k] = p;
			}
			else
			{
				p = iter->second;
			}
		}

		return p;
	}

	template<
		class LOCK,
		template<class, class, class, class> class Collection,
		class K,
		class T,
		class Traits = less<K>,
		class Allocator = std::allocator<pair<const K, T> >
	>
	size_t BSyncGetSize(LOCK& lock, Collection<K, T*, Traits, Allocator>& c)
	{
		size_t n = 0;
		{
			BScopeLock sl(&lock);
			n = c.size();
		}

		return n;
	}


}


#endif // _BSTDCOLLUTIL_H_