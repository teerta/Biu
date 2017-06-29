#include "stdafx.h"
#include "IODispatcher.h"
//#include "biodisp.h"
//#include "bmshack.h"
#include "MemoryShack.h"
#include "NetIOExecutionUnit.h"


using namespace System;
using namespace Biu;
using namespace Biu::Transmission;


// IODispatcher

IODispatcher::IODispatcher(MemoryShack^ shack, unsigned int numberOfIOs, unsigned int numberOfWorkers)
	: m_pDispatcher(NULL)
{
	m_pDispatcher = new BIODispatcher();
	if (!m_pDispatcher->Create(shack->m_pShack, numberOfIOs, numberOfWorkers))
	{
		throw gcnew InvalidOperationException("Failed: Create IODispatcher");
	}
}

IODispatcher::~IODispatcher()
{
	this->!IODispatcher();
}

IODispatcher::!IODispatcher()
{
	SAFE_DELETE(m_pDispatcher);
}

void IODispatcher::AddUnit(NetIOExecutionUnit^ unit)
{
	m_pDispatcher->AddUnit(unit->m_pUnit);
	unit->m_bDetached = true;
}
