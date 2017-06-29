
#include "stdafx.h"
#include "biodisp.h"


using namespace Biu;


// BIOExecutionUnit

BIOExecutionUnit::BIOExecutionUnit()
	: m_pDispatcher(NULL)
	//, m_pHandler(NULL)
	//, m_pProtocol(NULL)
{

}

BIOExecutionUnit::~BIOExecutionUnit()
{
	m_pDispatcher = NULL;
}


void BIOExecutionUnit::Reclaim(BIODispatchable* pDisp)
{
	delete pDisp;
}