
#ifndef _BTYPE_H_
#define _BTYPE_H_


#include "biu.h"
#include "bdef.h"
#include <string>


using namespace std;


namespace Biu {


	// TIMEVAR

	typedef union tagTIMEVAR
	{
			long            lt;
			long long		llt;
	} TIMEVAR, *LPTIMEVAR;


	// Class BCabUTF8String

	class BIU_API BCabUTF8String
	{
	public:
		BCabUTF8String();
		virtual ~BCabUTF8String();
		BCabUTF8String(const BCabUTF8String& str);
		BCabUTF8String(BCabUTF8String&& str);
		BCabUTF8String(const string& str);
		BCabUTF8String(const wchar_t* lpwsz);

//#ifdef _WINDOWS
//		BCabUTF8String(const CStringW& str);
//#endif

		inline const ssize_t GetLength() const { return m_cb; }
		inline ssize_t* LockLength() { return &m_cb; }
		inline char** LockData() { return &m_sz; }
		
		void SetString(const char* psz);
		void GetString(char* psz, size_t cb);

		BCabUTF8String& operator=(const BCabUTF8String& str);
		BCabUTF8String& operator=(BCabUTF8String&& str);
		BCabUTF8String& operator=(const wchar_t* lpwsz);
		BCabUTF8String& operator=(const string& str);

//#ifdef _WINDOWS
//		BCabUTF8String& operator=(const CStringW& str);
//#endif

		operator char*();
		operator const char*();

	#ifdef _WINDOWS
		int ToUnicode(LPWSTR* ppszWide);
		int ToUnicode(CAtlStringW& str);
	#endif

	protected:
		ssize_t						m_cb;
		char*						m_sz;

		void Copy(const BCabUTF8String& str);
	};


	// Struct BRawData

	struct BIU_API BRawData
	{
		BRawData();
		~BRawData();

		void Clear();
		void Set(unsigned char* pData, unsigned long cbData);

		inline bool HasData() { return NULL != data; }

		unsigned long cb;
		unsigned char* data;
	};
	
	
}


#endif // _BTYPE_H_
