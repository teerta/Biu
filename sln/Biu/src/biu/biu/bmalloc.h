
#ifndef _BMALLOC_H_
#define _BMALLOC_H_


#include "biu.h"


using namespace std;


namespace Biu {


	// Class BRoughAllocator

	template<class T, size_t SizeOfT = sizeof(T)>
	class BRoughAllocator
	{
	public:		
		T* Allocate(size_t nCount, size_t nBatch)
		{
			return static_cast<T*>(new char[SizeOfT * nCount * nBatch]);
		}

		void Deallocate(T* p)
		{
			delete[] p;
		}
	};


}


#endif // _BMALLOC_H_
