
#ifndef _BSORA_H_
#define _BSORA_H_


#include "bptl.h"


// Class BSora

class BIU_API BSora : public IBDataChannel
{
public:
	BSora();
	virtual ~BSora();
	
	/*
	 * ParseData checks pRestriction's Min & Max for pBrick's size.
	 * Subclass should override this method.
	 */
	virtual B_Data_Channel_Result ParseData(BMemoryBrick* pBrick, size_t& nDataPos, void* pSec = NULL, BProtocolRestriction* pRestriction = NULL);
	
	//////////////////////// Not Implemented //////////////////////////////
	virtual bool CreateObj(BMemoryBrick* pBrick, IBSerializable* pObj, void* pVal = NULL, void* pSec = NULL);
	virtual bool GetObj(IBSerializable* pObj);
	/////////////////////////////////////////////////////////////////////
	
	virtual void CreatePod(BMemoryBrick* pBrick, const void* pData, size_t cbData);
	virtual bool GetPod(void* pData, size_t& cbRead);
	virtual size_t GetPodSize();
	
	virtual IBDataChannel* Clone();
	virtual void ChangeMemory(BMemoryBrick* pBrick);    
	virtual void SetMemoryModel(BMemoryShack_char* pShack, BSerializer::Read_Function func);
	virtual BSerializer::Read_Function GetReadFunc();
	
	inline unsigned long long GetAction() { return m_ullAction; }
	
protected:
	BMemoryShack_char*                                          m_pShack;
	BSerializer::Read_Function								m_funcRead;
	BMemoryBrick*                                                   m_pRecvBrick;
	unsigned long long                                              m_ullAction;
	size_t                                                              m_cbRecv;
	size_t                                                              m_nPos;
	size_t                                                              m_nBrickPos;    
};



#endif // _BSORA_H_
