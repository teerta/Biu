
#ifndef _BSTRENC_H_
#define _BSTRENC_H_



#include "biu.h"



namespace Biu
{


	#ifdef _WINDOWS

	// Class BEncoder

	class BIU_API BEncoder
	{
	public:
		BEncoder();
		virtual ~BEncoder();

		/*
			Return -1 when error;
		*/
		static int ConvertToUTF8(LPCWSTR lpsz, LPSTR* ppszUTF8);
		static int ConvertToUTF8(CStringW& strWide, LPSTR* ppszUTF8);
		static int ConvertToUnicode(LPCSTR lpszUTF8, LPWSTR* ppsz);
		static int ConvertToUnicode(LPCSTR lpszUTF8, CStringW& strWide);
	};

	#endif // _WINDOWS


}


#endif // _BSTRENC_H_
