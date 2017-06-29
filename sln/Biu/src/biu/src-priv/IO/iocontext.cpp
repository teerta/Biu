
#include "stdafx.h"
#include "priv/bioctxt.h"
#include "biodef.h"
#include "bmshack.h"
#include "biohand.h"


using namespace Biu;


// BIOContext

BIOContext::BIOContext()
		: m_pDisp(NULL)
		, m_nDataPos(0)
		, m_pBrickHead(NULL)
#ifdef _WINDOWS
		, m_nTotalRead(0)
		, m_nTotalWrite(0)
		, m_pLastWriteBrick(NULL)
		, m_nLastBlockSize(0)
		//, m_hFile(INVALID_HANDLE_VALUE)
#endif
{
	m_operation = BIO_None;

#ifdef _WINDOWS
	m_wsaBuf.buf = m_buffer;
	m_wsaBuf.len = B_MAX_BUFF_SIZE;
#endif
}

BIOContext::BIOContext(BIODispatchable* pDisp)
		: m_pDisp(pDisp)
		, m_nDataPos(0)
		, m_pBrickHead(NULL)
{
	m_operation = BIO_None;
}

BIOContext::~BIOContext()
{
	m_pDisp->GetExecutionUnit()->GetDispatcher()->GetShack()->Push(m_pBrickHead);
}

BMemoryBrick* BIOContext::NewBrick()
{
	BMemoryBrick* pBrick = m_pDisp->GetExecutionUnit()->GetDispatcher()->GetShack()->Pop();
	if (m_pBrickHead)
	{
		m_pBrickHead->GetLastBrick()->SetNext(pBrick);
		
	}
	else
	{
		m_pBrickHead = pBrick;
	}
	
	return pBrick;
}

void BIOContext::PushBrick(BMemoryBrick* pBrick)
{
	BScopeLock sl(&m_csBrick);
	
	if (m_pBrickHead)
	{
		m_pBrickHead->GetLastBrick()->SetNext(pBrick);
	}
	else
	{
		m_pBrickHead = pBrick;
	}
}

void BIOContext::PushHeadBrick(BMemoryBrick* pBrick)
{
	BScopeLock sl(&m_csBrick);
	
	if (m_pBrickHead)
	{
		pBrick->SetNext(m_pBrickHead);
		m_pBrickHead = pBrick;
	}
	else
	{
		m_pBrickHead = pBrick;
	}
}

BMemoryBrick* BIOContext::PopBrick()
{
	BMemoryBrick* pBrick = NULL;
	{
		BScopeLock sl(&m_csBrick);
		if (m_pBrickHead)
		{
			pBrick = m_pBrickHead;
			m_pBrickHead = pBrick->GetNext();
			pBrick->SetNext(NULL);
		}
	}
	
	return pBrick;
}

BMemoryBrick* BIOContext::PeekBrick()
{
	BMemoryBrick* pBrick = NULL;
	{
		BScopeLock sl(&m_csBrick);
		if (m_pBrickHead)
		{
			pBrick = m_pBrickHead;
		}
	}

	return pBrick;
}

bool BIOContext::HasBricks()
{
	bool bRet = false;
	{
		BScopeLock sl(&m_csBrick);
		if (m_pBrickHead)
		{
			bRet = true;
		}
	}

	return bRet;
}

void BIOContext::Clear()
{
	m_operation = BIO_None;
	m_nDataPos = 0;
	m_pDisp->GetExecutionUnit()->GetDispatcher()->GetShack()->Push(m_pBrickHead);
	m_pBrickHead = NULL;
	memset(&m_Overlapped, 0, sizeof(m_Overlapped));
}

B_Data_Channel_Result BIOContext::ParseData()
{
	BScopeLock sl(&m_csBrick);
	return m_pDisp->GetProtocol()->ParseData(m_pBrickHead, m_nDataPos, m_pDisp->GetSecurityContext(), m_pDisp->GetHandler()->GetRestriction());
}

void BIOContext::ClearBrick()
{
	{
		BScopeLock sl(&m_csBrick);
		m_pDisp->GetExecutionUnit()->GetDispatcher()->GetShack()->Push(m_pBrickHead);
		m_pBrickHead = NULL;
	}
	m_nDataPos = 0;    
}

BMemoryBrick* BIOContext::AppeasePeerBrick()
{
	BMemoryBrick* pNew = NULL;
	{
		BScopeLock sl(&m_csBrick);
		pNew = m_pDisp->GetExecutionUnit()->GetDispatcher()->GetShack()->CutAndCopy(m_pBrickHead, m_nDataPos);
	}
	m_pDisp->GetProtocol()->ChangeMemory(pNew);

	return pNew;
}

//bool BIOContext::HasFile()
//{
//	bool bRet = false;
//	{
//		BScopeLock sl(&m_csFile);
//		if (INVALID_HANDLE_VALUE != m_hFile)
//		{
//			bRet = true;
//		}
//	}
//
//	return bRet;
//}
//
//bool BIOContext::OpenFile(LPCTSTR lpszFile, int id, int residentID)
//{
//	if (!PathFileExists(lpszFile))
//	{
//		return false;
//	}
//
//	bool bRet = false;
//	{
//		BScopeLock sl(&m_csFile);
//		if (INVALID_HANDLE_VALUE != m_hFile)
//		{
//			m_file = lpszFile;
//			m_hFile = CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
//			bRet = INVALID_HANDLE_VALUE != m_hFile;
//			if (bRet)
//			{
//				m_fileID = id;
//				m_fileResidentID = residentID;
//				LARGE_INTEGER cb;
//				if (GetFileSizeEx(m_hFile, &cb))
//				{
//					m_fileSize = cb.QuadPart;
//				}
//				else
//				{
//					CloseHandle(m_hFile);
//					m_hFile = INVALID_HANDLE_VALUE;
//					bRet = false;
//				}
//			}
//		}
//	}
//
//	return bRet;
//}
//
//void BIOContext::CloseFile()
//{
//	BScopeLock sl(&m_csFile);
//	if (INVALID_HANDLE_VALUE != m_hFile)
//	{
//		CloseHandle(m_hFile);
//		m_hFile = INVALID_HANDLE_VALUE;
//	}
//}
