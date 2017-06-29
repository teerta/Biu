#pragma once

#ifndef _BIU_H_
#define _BIU_H_


#if defined(_WINDOWS)

	#ifdef BIU_STATIC
			#define BIU_API	
	#else
			#ifdef BIU_EXPORTS
					#define BIU_API __declspec(dllexport)
			#else
					#define BIU_API __declspec(dllimport)
			#endif
	#endif

#elif defined(__linux__) || defined(__APPLE__)
	
	#define BIU_API
	#define EXTERN_C

	#ifndef __nullptr
		#define __nullptr nullptr
	#endif

#endif


namespace Biu {


	typedef struct tagBHANDLE{  } *BHANDLE;


}


#endif // _BIU_H_
