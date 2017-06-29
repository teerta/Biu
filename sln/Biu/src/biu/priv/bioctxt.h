
#ifndef _BIOCTXT_H_
#define _BIOCTXT_H_


#include "biu.h"
#include "bsyncprim.h"
#include "bptl.h"
#include "bbase.h"
#include "biodisp.h"
#include <list>
#include <atomic>


using namespace std;


namespace Biu {


	class BIODispatchable;
	struct BMemoryBrick;


	// Struct BIOContext

	struct BIOContext
	{
		BIOContext();
		BIOContext(BIOContext& xioc) = delete;
		BIOContext(BIODispatchable* pDisp);		
		~BIOContext();

		BIOContext& operator=(BIOContext& xioc) = delete;
		
		BMemoryBrick* NewBrick();
		void PushBrick(BMemoryBrick* pBrick);
		void PushHeadBrick(BMemoryBrick* pBrick);
		BMemoryBrick* PopBrick();
		void Clear();        
		B_Data_Channel_Result ParseData();
		void ClearBrick();
		BMemoryBrick* AppeasePeerBrick();
		BMemoryBrick* PeekBrick();
		bool HasBricks();

		//bool HasFile();
		//bool OpenFile(LPCTSTR lpszFile, int id, int residentID);
		//void CloseFile();

#ifdef _WINDOWS
		//void MaskOperation(B_IO_Operation op);
		//void UnmaskOperation(B_IO_Operation op);
		//unsigned int GetOperation();
		//void ZeroOperation();

		inline void Work() { m_pDisp->GetExecutionUnit()->ExecuteWorker(this); }
		//inline void Io(LPOVERLAPPED_ENTRY poe, BOOL bSucc) { m_pDisp->GetExecutionUnit()->ExecuteIO(poe, bSucc); }
		inline void Io(DWORD cb, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped, BOOL bSucc)
		{
			m_pDisp->GetExecutionUnit()->ExecuteIO(cb, lpCompletionKey, lpOverlapped, bSucc);
		}
#else
		inline bool Work(shared_ptr<list<BIOContext*> >& listPending) { return m_pDisp->GetExecutionUnit()->ExecuteWorker(this, listPending); }
#endif


#ifdef _WINDOWS
		// Overlapped Part

		WSAOVERLAPPED m_Overlapped;
#endif
		
		// Members

		BCriticalSection m_csBrick;
		
		BIODispatchable* m_pDisp;
		atomic_uint m_operation;
		size_t m_nDataPos;
		BMemoryBrick* m_pBrickHead;

#ifdef _WINDOWS
		char m_buffer[B_MAX_BUFF_SIZE];
		WSABUF m_wsaBuf;
		SSIZE_T m_nTotalWrite;
		SSIZE_T m_nTotalRead;
		BMemoryBrick* m_pLastWriteBrick;
		SSIZE_T m_nLastBlockSize;
#endif

		//BCriticalSection m_csFile;
		//HANDLE m_hFile;
		//CString m_file;
		//unsigned long long m_fileSize;
		//int m_fileID;
		//int m_fileResidentID;
	};


}


#endif // _BIOCTXT_H_
