
#ifndef _BSTRING_H_


#include "biu.h"
#include <stddef.h>

#ifdef _WINDOWS
#include <winnt.h>
#endif


namespace Biu {
	

#ifdef _WINDOWS


	EXTERN_C
	BIU_API
	LPTSTR* PrepareArgs(LPTSTR psz, LPCTSTR sep = _T(" "), int* pArgc = NULL);

	EXTERN_C
	BIU_API
	LPTSTR GetDir(LPTSTR psz, LPTSTR pDir, unsigned int cbDir);


#else

	
	char** PrepareArgs(char* psz, const char* sep = " ", int* pArgc = NULL);

	char* GetDir(char* psz, char* pDir, unsigned int cbDir);


#endif

		
}


#endif // _BSTRING_H_
