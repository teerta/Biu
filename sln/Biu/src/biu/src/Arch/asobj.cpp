
#include "stdafx.h"
#include "basobj.h"


using namespace Biu;


// Biu_DataHeader

Biu_DataHeader::Biu_DataHeader()
	: m_ID(0)
	, m_Current(0)
	, m_Total(0)
	, m_BufferSize(0)
{
}

Biu_DataHeader::~Biu_DataHeader()
{
}

bool Biu_DataHeader::BSerialize(bool bStoring, BSerializer& bms)
{
	if (bStoring)
	{		
		bms.Serialize(m_ID);
		bms.Serialize(m_Current);
		bms.Serialize(m_Total);
		bms.Serialize(m_BufferSize);
	}
	else
	{
		BSBR(bms.Deserialize(m_ID));
		BSBR(bms.Deserialize(m_Current));
		BSBR(bms.Deserialize(m_Total));
		BSBR(bms.Deserialize(m_BufferSize));
	}

	return true;
}
