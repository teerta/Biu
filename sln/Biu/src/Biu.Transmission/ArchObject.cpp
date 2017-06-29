#include "stdafx.h"
#include "ArchObject.h"


using namespace Biu::Transmission;


// BiuDataHeader

BiuDataHeader::BiuDataHeader()
	: m_ID(0)
	, m_Total(0)
	, m_Current(0)
	, m_BufferSize(0)
{

}

BiuDataHeader::~BiuDataHeader()
{

}

bool BiuDataHeader::BSerialize(BiuSerializer^ ser)
{	
	bool bRet = true;
	try
	{
		ser
			->Serialize(m_ID)
			->Serialize(m_Total)
			->Serialize(m_Current)
			->Serialize(m_BufferSize);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool BiuDataHeader::BDeserialize(BiuSerializer^ ser)
{
	bool bRet = true;
	try
	{
		ser
			->Deserialize(m_ID)
			->Deserialize(m_Total)
			->Deserialize(m_Current)
			->Deserialize(m_BufferSize);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}
