
#ifndef _BMSHACK_H_
#define _BMSHACK_H_


#include "biu.h"

#include <string.h>
#include <memory>
#include <stdexcept>
#include <queue>
#include <list>
#include <algorithm>

#include "bmarco.h"
#include "bmalloc.h"
#include "bspfunc.h"
#include "bsyncprim.h"


using namespace std;


namespace Biu {
	
	// Forwarding

	struct BMemoryBrick;



	// Functions 

	// 1-Based
	size_t BFindBrickPositionByData(BMemoryBrick* pBrick, size_t nDataPos);

	BMemoryBrick* BFindBrickXY(BMemoryBrick* pBrick, size_t& nDataPos);

	BMemoryBrick* BFindBrickPrevious(BMemoryBrick* pBrick, BMemoryBrick* pHead);



	struct BIU_API BMemoryBrick
	{
	public:
		BMemoryBrick(char* pLoc, size_t cbBrick);
		BMemoryBrick(const BMemoryBrick& rhs);
		
		BMemoryBrick& operator=(const BMemoryBrick& rhs);
		
		void Copy(const BMemoryBrick& rhs);

		inline char* GetMemory() { return m_pLoc; }
		inline char* GetEndMemory() { return m_pLoc + m_cbBrick - 1; }
		inline char* GetOffsetMemory(size_t nOffset) { return GetMemory() + nOffset; }
		inline char* GetFreeMemory() { return m_pLoc + GetDataSize(); }
		
		inline const size_t GetSize() const { return m_cbBrick; }
		inline const size_t GetFreeSize() const { return m_nFreeSize; }
		inline const size_t GetDataSize() const { return GetSize() - GetFreeSize(); }
		inline void SetDataSize(const size_t cb) { m_nFreeSize = m_cbBrick - cb; }
		
		inline void ResetFreeSize() { m_nFreeSize = GetSize(); }
		inline void IncFreeSize(const size_t cb) { m_nFreeSize += cb; }
		inline void DecFreeSize(const size_t cb) { m_nFreeSize -= cb; }		
		
		inline bool IsEndOfSeries() { return m_bIsEndOfSeries; }
		inline void SetEndOfSeries(bool bEndOfSeries) { m_bIsEndOfSeries = bEndOfSeries; }

		inline void* GetCounterpart() { return m_pCounterpart; }
		inline void SetCounterpart(void* p) { m_pCounterpart = p; }

		inline BMemoryBrick* GetNext() { return m_pNext; }
		inline void SetNext(BMemoryBrick* p) { m_pNext = p; }
		inline void ResetNext() { m_pNext = NULL; }

		inline bool IsFull() const { return 0 == m_nFreeSize; }
		
		inline void Clear()
		{
			ResetFreeSize();
			ResetNext();
			m_bIsEndOfSeries = false;
			m_pCounterpart = NULL;
		}
		
		inline BMemoryBrick* GetLastBrick()
		{
			BMemoryBrick* p = this;
			while (p->GetNext())
			{
				p = p->GetNext();
			}

			return p;
		}
		
		inline size_t UntilGetDataSize()
		{
			size_t cb = 0;
			BMemoryBrick* p = this;
			while (p)
			{
				cb += p->GetDataSize();
				p = p->GetNext();
			}
			
			return cb;
		}
		
		inline size_t UntilGetSize()
		{
			size_t cb = 0;
			BMemoryBrick* p = this;
			while (p)
			{
				cb += p->GetSize();
				p = p->GetNext();
			}
			
			return cb;
		}

	private:
		char*									m_pLoc;
		size_t									m_cbBrick;
		size_t									m_nFreeSize;
		bool									m_bIsEndOfSeries;
		void*									m_pCounterpart;
		BMemoryBrick*							m_pNext;
	};


	// Class BMemoryShack

	template<class T = char, class Alloc = BRoughAllocator<T> >
	class BMemoryShack
	{
	public:

		typedef T		Type;
		typedef T*		Type_Pointer;
		typedef T&		Type_Reference;
		typedef Alloc   Allocator;

		BMemoryShack(size_t nChunkSize = 1024, size_t nCount = 1000)
			: m_nChunkSize(nChunkSize)
			, m_nChunkCount(nCount)
		{
			if (m_nChunkSize <= Size_Of_Type || m_nChunkSize < 100)
			{
				throw length_error("Chunk size is too pity.");
			}

			NewWall();			
		}

		virtual ~BMemoryShack()
		{
			for_each(begin(m_listWall), end(m_listWall), [=](Type_Pointer p)
			{
				m_allocator.Deallocate(p);
			});
		}

		BMemoryBrick* Pop()
		{
			//INFINITE_SPIN(m_bManaging);

			BScopeLock sl(&m_csBrick);

			if (m_qBrick.empty())
			{
				NewWall();
			}

			auto p = m_qBrick.front();
			m_qBrick.pop();

			return p;
		}

		void Push(BMemoryBrick* p)
		{
			if (!p)
			{
				return;
			}
			
			BScopeLock sl(&m_csBrick);
			
			do 
			{
				BMemoryBrick* pNext = p->GetNext();
				p->Clear();
				m_qBrick.push(p);
				p = pNext;
			} while(p);
		}
		
		BMemoryBrick* PushBreak(BMemoryBrick* p, BMemoryBrick* pBreak)
		{
			if (!p)
			{
				return __VNULL;
			}
			
			BScopeLock sl(&m_csBrick);
			
			do 
			{
				if (pBreak == p)
				{
					return p;
				}
				
				BMemoryBrick* pNext = p->GetNext();
				p->Clear();
				m_qBrick.push(p);
				p = pNext;
			} while(p);
			
			return p;
		}
		
		BMemoryBrick* PushBreak(BMemoryBrick* p, size_t& nDataPos)
		{
			if (!p)
			{
				return __VNULL;
			}
			
			BScopeLock sl(&m_csBrick);
			
			BMemoryBrick* pBrick = p;
			while (pBrick && nDataPos > pBrick->GetDataSize())
			{
				nDataPos -= pBrick->GetDataSize();
				BMemoryBrick* pCur = pBrick;
				pBrick = pBrick->GetNext();
				pCur->Clear();
				m_qBrick.push(pCur);
			}

			return pBrick;
		}
		
		// Retuen chain of BMemoryBricks that matches the initial nDataPos

		BMemoryBrick* CutAndCopy(BMemoryBrick*& p, size_t& nDataPos)
		{      
			BMemoryBrick* pBrick = p;
			BMemoryBrick* pLast = NULL;
			while (p && nDataPos >= p->GetDataSize())
			{
				nDataPos -= p->GetDataSize();
				pLast = p;
				p = p->GetNext();
			}
			
			if (nDataPos)
			{
				// More data are available
				/*
					Copy the last Brick that contains the matching data.
					nDataPos is the new initial position for next reading of p.
				*/

				if (p)
				{
					BMemoryBrick* pNew = Pop();
					pNew->Copy(*p);
					pNew->SetNext(NULL);
					if (pLast)
					{
						pLast->SetNext(pNew);
					}
					else
					{
						pBrick = pNew;
					}
				}

/*				BMemoryBrick* pNew = Pop();
				pNew->Copy(*p);
				pNew->SetNext(NULL);
				if (pLast)
				{
					pLast->SetNext(pNew);
				}
				else
				{
					pBrick = pNew;
				} */               
			}
			else
			{
				// All data have been handled
				/*					
					Cut p
				*/

				if (pLast)
				{					
					pLast->SetNext(NULL);
				}

/*				if (pLast)
				{
					pLast->SetNext(NULL);
				}
				else
				{
					p = p->GetNext();
					pBrick->SetNext(NULL);
				} */               
			}
						
			return pBrick;
		}

		bool Replace(BMemoryBrick* pSrc, BMemoryBrick* pNew, size_t nBegin, size_t nEnd)
		{
			assert(pSrc);
			assert(pNew);

			size_t nTotal = pSrc->UntilGetSize();
			if (nBegin < nEnd || nBegin > nTotal || nEnd > nTotal)
			{
				return false;
			}

			size_t nPos1 = nBegin;			
			auto* p1 = BFindBrickXY(pSrc, nPos1);
			if (nBegin == nEnd)
			{
				// Replace
				/*
					p1(Cut) ---> pNew
					Push bricks in the range of (p1, Next]
				*/

				if (p1->GetNext())
				{
					Push(p1->GetNext());
				}

				p1->SetDataSize(nPos1);
				p1->SetNext(pNew);
			}
			else
			{
				// Replace & Append Remaining
				/*
					Same Brick (p1 == p2)
					p1(Cut) ---> pNew ---> pMid(p2's Cut) ---> p1->Next

					Different Brick (p1 != p2)
					The same as above, and Push bricks in the range of (p1, p2]
				*/

				size_t nPos2 = nEnd;
				auto* p2 = BFindBrickXY(pSrc, nPos2);

				if (p1 != p2)
				{
					// Different Bricks

					for (BMemoryBrick* pi = p1->GetNext(); pi != p2;)
					{
						auto* pt = pi->GetNext();
						pi->ResetNext();
						Push(pi);
						pi = pt;
					}
				}

				auto* pMid = Pop();
				BWriteBrick(pMid, this, p2->GetOffsetMemory(nPos2), p2->GetDataSize() - nPos2);
				pMid->SetNext(p2->GetNext());
				pNew->SetNext(pMid);
				p1->SetDataSize(nPos1);
				p1->SetNext(pNew);

				if (p1 != p2)
				{
					// Different Bricks

					p2->ResetNext();
					Push(p2);
				}
			}

			return true;
			
		}

		inline const size_t GetChunkSize() const { return m_nChunkSize; }
		inline const size_t GetChunkCount() const { return m_nChunkCount; }

	private:
		static const size_t Size_Of_Type = sizeof(Type);

		void NewWall()
		{
			static const size_t Size_Of_MemoryBrick = sizeof(BMemoryBrick);
			
			Type_Pointer p = m_allocator.Allocate(m_nChunkSize, m_nChunkCount);

			//m_bManaging = true;

			m_listWall.push_back(p);
			BMemoryBrick* pBrick = NULL;
			char* pLoc = NULL;
			{
				//BScopeLock sl(&m_csBrick);
				for (size_t i = 0; i < m_nChunkCount; i++)
				{
					pLoc = p + i * (Size_Of_Type * m_nChunkSize);
					pBrick = new (pLoc) BMemoryBrick(pLoc + Size_Of_MemoryBrick, m_nChunkSize - Size_Of_MemoryBrick);
					m_qBrick.push(pBrick);
				}
			}

			//m_bManaging = false;
		}

		Allocator m_allocator;

		size_t m_nChunkSize;
		size_t m_nChunkCount;		

		list<Type_Pointer> m_listWall;
		queue<BMemoryBrick*> m_qBrick;

		BCriticalSection m_csBrick;
	};

	
	// Functions
	
	template<class T = char, class Shack = BMemoryShack<T> >
	void BReserveBrick(BMemoryBrick* pMemoryBrick, Shack* pShack, size_t cbData)
	{
		static const size_t Size_Of_Type = sizeof(T);

		size_t cbMem = cbData * Size_Of_Type;
		BMemoryBrick* pBrick = pMemoryBrick->GetLastBrick();
		size_t cbWrite = 0;
		while (cbMem)
		{
			if (pBrick->IsFull())
			{
				pBrick->SetNext(pShack->Pop());
				pBrick = pBrick->GetNext();
			}

			size_t cbOp = min<size_t>(cbMem, pBrick->GetFreeSize());
			pBrick->DecFreeSize(cbOp);
			cbWrite += cbOp;
			cbMem -= cbOp;
		}
	}
		
	template<class T = char, class Shack = BMemoryShack<T> >
	void BWriteBrick(BMemoryBrick* pMemoryBrick, Shack* pShack, const T* pData, size_t cbData)
	{
		static const size_t Size_Of_Type = sizeof(T);

		size_t cbMem = cbData * Size_Of_Type;
		BMemoryBrick* pBrick = pMemoryBrick->GetLastBrick();
		size_t cbWrite = 0;
		while (cbMem)
		{
			if (pBrick->IsFull())
			{
				pBrick->SetNext(pShack->Pop());
				pBrick = pBrick->GetNext();
			}

			size_t cbOp = min<size_t>(cbMem, pBrick->GetFreeSize());

			char* pMem = pBrick->GetFreeMemory();
			memcpy(pMem, (const char*)pData + cbWrite,  cbOp);
			pBrick->DecFreeSize(cbOp);
			cbWrite += cbOp;
			cbMem -= cbOp;
		}
	}
	
	template<class T = char, class Shack = BMemoryShack<T> >
	void BWriteReservedBrick(BMemoryBrick* pMemoryBrick, Shack* pShack, size_t nPos, const T* pData, size_t cbData)
	{
		static const size_t Size_Of_Type = sizeof(T);

		BMemoryBrick* pBrick = pMemoryBrick;
		size_t nIdx = nPos / pBrick->GetSize();
		size_t nOffset = nPos % pBrick->GetSize();
		while (nIdx--)
		{
			pBrick = pBrick->GetNext();
		}
		
		size_t cbMem = cbData * Size_Of_Type;
		size_t cbWrite = 0;
		
		while (cbMem)
		{
			char* pMem = pBrick->GetOffsetMemory(nOffset);
			size_t cbOp = min<size_t>(cbMem, pBrick->GetSize() - nOffset);        
			memcpy(pMem, (const char*)pData + cbWrite,  cbOp);
			cbWrite += cbOp;
			cbMem -= cbOp;
			
			if (cbMem)
			{
				pBrick = pBrick->GetNext();
				nOffset = 0;
			}
		}
	}
	
	template<class T = char>
	bool BReadBrick(BMemoryBrick* pMemoryBrick, T* pBuff, size_t cbBuff, size_t& nPos)
	{
		static const size_t Size_Of_Type = sizeof(T);

		cbBuff *= Size_Of_Type;
		BMemoryBrick* pBrick = pMemoryBrick;
		if (pBrick->UntilGetDataSize() - nPos < cbBuff)
		{
			return false;
		}

		size_t nIdx = nPos / pBrick->GetSize();
		size_t nOffset = nPos % pBrick->GetSize();
		while (nIdx--)
		{
			pBrick = pBrick->GetNext();
			CHK_false(pBrick);
		}

		size_t cbRead = 0;
		while (cbBuff)
		{
			char* pMem = pBrick->GetOffsetMemory(nOffset);
			size_t cbOp = min<size_t>(cbBuff, pBrick->GetSize() - nOffset);
			cbBuff -= cbOp;                
			memcpy((char*)pBuff + cbRead, pMem, cbOp);
			cbRead += cbOp;

			if (cbBuff)
			{
				pBrick = pBrick->GetNext();
				nOffset = 0;
				CHK_false(pBrick);
			}
		}

		nPos += cbRead;
		return true;
	}
			
//    template<>
//    bool BReadBrick<char>(BMemoryBrick* pMemoryBrick, char* pBuff, size_t cbBuff, size_t& nPos);
	
	template<class T = char>
	bool BReadBrickSparsely(BMemoryBrick* pMemoryBrick, T* pBuff, size_t cbBuff, size_t& nPos)
	{
		static const size_t Size_Of_Type = sizeof(T);

		if (!cbBuff)
		{
			return true;
		}

		cbBuff *= Size_Of_Type;
		BMemoryBrick* pBrick = pMemoryBrick;
		if (pBrick->UntilGetDataSize() - nPos < cbBuff)
		{
			return false;
		}

		size_t nOffset = nPos;
		while (nOffset >= pBrick->GetDataSize())
		{
			nOffset -= pBrick->GetDataSize();
			pBrick = pBrick->GetNext();
			CHK_false(pBrick);
		}

		size_t cbRead = 0;
		while (cbBuff)
		{
			char* pMem = pBrick->GetOffsetMemory(nOffset);
			size_t cbOp = min<size_t>(cbBuff, pBrick->GetDataSize() - nOffset);
#ifdef _WINDOWS
			memcpy_s((char*)pBuff + cbRead, cbBuff, pMem, cbOp);
#else
			memcpy((char*)pBuff + cbRead, pMem, cbOp);
#endif			
			cbBuff -= cbOp;
			cbRead += cbOp;

			if (cbBuff)
			{
				pBrick = pBrick->GetNext();
				nOffset = 0;
				CHK_false(pBrick);
			}
		}

		nPos += cbRead;
		return true;
	}

	template<class T = char>
	size_t BReadBrickSparselyUnchecked(BMemoryBrick* pMemoryBrick, T* pBuff, size_t cbBuff, size_t nStopPos, size_t& nPos)
	{
		static const size_t Size_Of_Type = sizeof(T);

		cbBuff *= Size_Of_Type;
		BMemoryBrick* pBrick = pMemoryBrick;
		size_t nOffset = nPos;
		while (nOffset >= pBrick->GetDataSize())
		{
			nOffset -= pBrick->GetDataSize();
			pBrick = pBrick->GetNext();
			if (!pBrick)
			{
				return 0;
			}
		}

		size_t cbRead = 0;
		while (cbBuff || (nStopPos && nPos + cbRead >= nStopPos))
		{
			char* pMem = pBrick->GetOffsetMemory(nOffset);
			size_t cbOp = min<size_t>(cbBuff, pBrick->GetDataSize() - nOffset);
			if (nStopPos)
			{
				cbOp = min<size_t>(cbOp, nStopPos - nPos - cbRead);
			}

			cbBuff -= cbOp;
			memcpy((char*)pBuff + cbRead, pMem, cbOp);
			cbRead += cbOp;

			if (cbBuff)
			{
				pBrick = pBrick->GetNext();
				nOffset = 0;
				if (!pBrick)
				{
					break;
				}
			}
		}

		nPos += cbRead;
		return cbRead;
	}

#ifdef _WINDOWS
	template<class T = char, class Shack = BMemoryShack<T> >
	bool BDumpBrickToFile(BMemoryBrick* pMemoryBrick, Shack* pShack, HANDLE hFile)
	{
		static const size_t Size_Of_Type = sizeof(T);

		size_t cb = 0;
		BMemoryBrick* pBrick = pMemoryBrick;
		while (pBrick)
		{
			if (!WriteFile(hFile, pBrick->GetMemory(), pBrick->GetDataSize(), &cb, NULL))
			{
				return false;
			}

			pBrick = pBrick->GetNext();
		}

		return true;
	}
#endif // _WINDOWS
	
//    template<>
//    bool BReadBrickSparsely<char>(BMemoryBrick* pMemoryBrick, char* pBuff, size_t cbBuff, size_t& nPos);
	
	
	// typedefs
	
	typedef BMemoryShack<char> BMemoryShack_char;
	
	inline bool BReadBrickSparsely_char(BMemoryBrick* pMemoryBrick, char* pBuff, size_t cbBuff, size_t& nPos)
	{
		return BReadBrickSparsely(pMemoryBrick, pBuff, cbBuff, nPos);
	}


	inline size_t BReadBrickSparselyUnchecked_char(BMemoryBrick* pMemoryBrick, char* pBuff, size_t cbBuff, size_t nStopPos, size_t& nPos)
	{
		return BReadBrickSparselyUnchecked(pMemoryBrick, pBuff, cbBuff, nStopPos, nPos);
	}

}


#endif // _BMSHACK_H_
