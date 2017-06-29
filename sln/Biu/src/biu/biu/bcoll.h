
#ifndef _BCOLL_H_
#define _BCOLL_H_


#include "biu.h"
#include "bmarco.h"
#include "bexception.h"
#include <map>


using namespace std;


namespace Biu {


	// Functions 
	
	EXTERN_C
	BIU_API
	unsigned int 
	CreateKey_RandomNumber();


	// Class BCookieMap

	template<class V>
	class BCookieMap
	{
	public:
		typedef map<unsigned int, V> Type;
		typedef typename Type::iterator Iterator;
		typedef pair<unsigned int, V> Pair;

		BCookieMap()
		{
		}

		virtual ~BCookieMap()
		{
		}

		inline const size_t GetCount() const
		{
				return m_map.size();
		}

		inline Iterator Begin()
		{
				return m_map.begin();
		}

		inline const Iterator ConstBegin()
		{
				return m_map.begin();
		}

		inline Iterator End()
		{
				return m_map.end();
		}

		inline const Iterator ConstEnd()
		{
				return m_map.end();
		}

		const unsigned int Add(V& val)
		{
				unsigned int nCookie = CreateKey_RandomNumber();
				while (m_map.end() != m_map.find(nCookie))
				{
						nCookie = CreateKey_RandomNumber();
				}

				m_map[nCookie] = val;

				return nCookie;
		}

		void Remove(const unsigned int nCookie)
		{
				Iterator iter = m_map.find(nCookie);
				CHK(m_map.end() != iter);

				m_map.erase(iter);
		}

		void RemovePointer(const unsigned int nCookie)
		{
				Iterator iter = m_map.find(nCookie);
				CHK(m_map.end() != iter);

				delete iter->second;
				m_map.erase(iter);
		}

		void RemovePointerArray(const unsigned int nCookie)
		{
				Iterator iter = m_map.find(nCookie);
				CHK(m_map.end() != iter);

				delete[] iter->second;
				m_map.erase(iter);
		}

		inline void Clear()
		{
				m_map.clear();
		}

		inline bool ContainsKey(const unsigned int nCookie)
		{
				return m_map.end() != m_map.find(nCookie);
		}

		bool GetValue(const unsigned int nCookie, V& v)
		{
				Iterator iter = m_map.find(nCookie);
				if (m_map.end() != iter)
				{
						v = iter->second;
						return true;
				}

				return false;
		}

		template<class Func>
		inline void ForEach(Func func)
		{
				for_each(m_map.begin(), m_map.end(), func);
		}

		template<class Func>
		inline void ClearPointer(Func func)
		{
				for_each(m_map.begin(), m_map.end(), func);
				m_map.clear();
		}

		inline void ClearPointer()
		{
				for_each(m_map.begin(), m_map.end(), 
						[](Pair pair)
						{
								delete pair.second;
						}
						);

				m_map.clear();
		}

		V& operator [] (const unsigned int nCookie)
		{
				Iterator iter = m_map.find(nCookie);
				if (m_map.end() == iter)
				{
						throw BE_NotFound();
				}

				return iter->second;
		}

	private:
		map<unsigned int, V> m_map;
	};


}


#endif // _BCOLL_H_
