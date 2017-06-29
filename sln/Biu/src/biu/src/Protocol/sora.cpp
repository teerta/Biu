
#include "stdafx.h"
#include "bsora.h"
#include <cassert>


// BSora

BSora::BSora()
	: m_pShack(NULL)
	, m_pRecvBrick(NULL)
	, m_ullAction(0ull)
	, m_cbRecv(0)
	, m_nPos(0)
	, m_nBrickPos(0)
{    
}

BSora::~BSora()
{    
}

B_Data_Channel_Result BSora::ParseData(BMemoryBrick* pBrick, size_t& nDataPos, void* pSec, BProtocolRestriction* pRestriction)
{
	assert(pBrick);
	
	B_Data_Channel_Result result = B_DC_R_Succeeded;
	size_t cbData = pBrick->UntilGetDataSize();
	if (pRestriction)
	{
		if (pRestriction->GetMinPacketSize())
		{            
			if (pRestriction->GetMinPacketSize() > cbData)
			{
				result = B_DC_R_Continue;
			}
			
			if (cbData > pRestriction->GetMaxPacketSize())
			{
				nDataPos += pRestriction->GetMaxPacketSize();
				result = B_DC_R_More;
			}
		}
	}
	
	m_pRecvBrick = pBrick;
	m_nBrickPos = nDataPos;
	m_cbRecv = cbData;
	m_nPos = 0;    
	
	return result;
}

bool BSora::CreateObj(BMemoryBrick* pBrick, IBSerializable* pObj, void* pVal, void* pSec)
{
	throw runtime_error("Not Implemented");
	return false;
}

bool BSora::GetObj(IBSerializable* pObj)
{
	throw runtime_error("Not Implemented");
	return false;
}

void BSora::CreatePod(BMemoryBrick* pBrick, const void* pData, size_t cbData)
{
	assert(pBrick);
	assert(pData);
	assert(cbData);
	
	BWriteBrick(pBrick, m_pShack, (const char*)pData, cbData);
}

bool BSora::GetPod(void* pData, size_t& cbRead)
{
	assert(pData);
	
	if (!m_cbRecv || !(m_cbRecv - m_nPos))
	{
		return false;
	}
	
	size_t nOldBrickPos = m_nBrickPos;
	if (!m_funcRead(m_pRecvBrick, (char*)pData, m_cbRecv - m_nPos, m_nBrickPos))
	{
		return false;
	}
	
	cbRead = m_nBrickPos - nOldBrickPos;
	m_nPos += cbRead;
	
	return true;
}

size_t BSora::GetPodSize()
{
	return m_cbRecv;
}

IBDataChannel* BSora::Clone()
{
	BSora* p = new BSora();
	p->SetMemoryModel(m_pShack, m_funcRead);

	return p;
}

void BSora::ChangeMemory(BMemoryBrick* pBrick)
{
	assert(pBrick);
	
	m_pRecvBrick = pBrick;
}

void BSora::SetMemoryModel(BMemoryShack_char* pShack, BSerializer::Read_Function func)
{
	m_pShack = pShack;
	m_funcRead = func;
}

BSerializer::Read_Function BSora::GetReadFunc()
{
	return m_funcRead;
}
