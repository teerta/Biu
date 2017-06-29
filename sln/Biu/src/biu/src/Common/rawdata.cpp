
#include "stdafx.h"
#include "btype.h"
#include "bmarco.h"


using namespace Biu;


BRawData::BRawData()
	: cb(0)
	, data(NULL)
{

}

BRawData::~BRawData()
{
	SAFE_DELETE_ARRAY(data);
}

void BRawData::Clear()
{
	SAFE_DELETE_ARRAY(data);
	cb = 0;
}

void BRawData::Set(unsigned char* pData, unsigned long cbData)
{
	Clear();
	data = new unsigned char[cbData];
	CopyMemory(data, pData, cbData);
	cb = cbData;
}
