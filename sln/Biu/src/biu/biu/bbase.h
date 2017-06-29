
#ifndef _BBASE_H_
#define _BBASE_H_


#include "biu.h"
#include <stddef.h>


namespace Biu {


	#define B_MAX_BUFF_SIZE					4096
	#define B_MAX_HOST_SIZE					64
	#define B_MAX_WORKER_THREAD				1024
	#define B_MAX_NAME_SIZE					256


	extern const size_t	BIU_API				B_Size_Of_BYTE;
	extern const size_t BIU_API				B_Size_Of_WORD;
	extern const size_t BIU_API				B_Size_Of_DWORD;
	extern const size_t BIU_API				B_Size_Of_QWORD;

	extern const size_t BIU_API				B_Size_Of_SockAddr_In;
	extern const size_t BIU_API				B_Size_Of_SockAddr_Storage;

	#ifdef _WINDOWS

	extern const size_t BIU_API				B_Size_Of_Socket;
	extern const size_t BIU_API				B_Size_Of_FileTime;
	extern const size_t BIU_API				B_Size_Of_Overlapped;
	extern const size_t BIU_API				B_Size_Of_AcceptEx_Addr;
	extern const size_t BIU_API				B_Size_Of_AcceptEx_Available_Buffer;

	#endif // _WINDOWS


	//#define B_COPYMEMORY_IDB_NAME				__B_dwCmidx__
	//#define B_BEGIN_COPYMEMORY()				DWORD B_COPYMEMORY_IDB_NAME = 0;
	//#define B_END_COPYMEMORY()					// dummy
	//#define B_COPYMEMORY_ENTRY_P(p, b, s)		CopyMemory((p), ((LPBYTE)(b)) + B_COPYMEMORY_IDB_NAME, (s)); B_COPYMEMORY_IDB_NAME += (s);
	//#define B_COPYMEMORY_ENTRY(p, b, s)			B_COPYMEMORY_ENTRY_P(&p, b, s);


}


#endif // _BBASE_H_
