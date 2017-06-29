
#ifndef _BSPFUNC_H_
#define _BSPFUNC_H_


#include "biu.h"


#if defined(_WINDOWS)        
		#include <intrin.h>
#elif defined(__linux__) || defined(__APPLE__)
		#include <pthread.h>
#endif


namespace Biu {

	#ifdef _WINDOWS
		#define INFINITE_SPIN_WINDOWS(cond) \
				while ((cond)) \
				{ \
						YieldProcessor(); \
				}
	#endif // _WINDOWS

	#if defined(__linux__) || defined(__APPLE__)
		#define INFINITE_SPIN_LINUX(cond) \
				while ((cond)) \
				{ \
						pthread_yield(); \
				}
	#endif // __linux__


	#if defined(_WINDOWS)
			#define INFINITE_SPIN	INFINITE_SPIN_WINDOWS
	#elif defined(__linux__) || defined(__APPLE__)
			#define INFINITE_SPIN	INFINITE_SPIN_LINUX
	#endif // _WINDOWS

}


#endif // _BSPFUNC_H_
