
#include "stdafx.h"
#include "biodisp.h"
#include "priv/i_iodispatcher.h"
#include "biodef.h"
#include "bbfunc.h"
#include "bsyncprim.h"
#include "priv/bioctxt.h"
#include "bstdcollutil.h"
#include "bbfunc.h"
#include <memory>
#include <algorithm>
#include <cassert>

#ifndef _WINDOWS
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <bits/signum.h>
#include <sys/epoll.h>
#endif


using namespace std;
using namespace Biu;


// BIODispatcher

#ifndef _WINDOWS
pthread_key_t BIODispatcher::TSK_Return_Brick;
#endif

BIODispatcher::BIODispatcher()
	: m_pIntern(NULL)
{
	m_pIntern = (BHANDLE)(new I_IODispatcher());
}

BIODispatcher::~BIODispatcher()
{
	Close();
}

bool BIODispatcher::Create(BMemoryShack_char* pShack, unsigned int ulIOs, unsigned int ulWorkers)
{
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);

	return pIntern->Create(pShack, ulIOs, ulWorkers);
}

void BIODispatcher::Close()
{
	CHK(m_pIntern);

	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);
	pIntern->Close();
	delete pIntern;
	m_pIntern = NULL;
}

bool BIODispatcher::JoinTheCircle(BIODispatchable* pDisp)
{
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);
	return pIntern->JoinTheCircle(pDisp);	
}

#ifndef _WINDOWS
bool BIODispatcher::JoinTheCircleForWriting(BIODispatchable* pDisp)
{
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);
	return pIntern->JoinTheCircleForWriting(pDisp);	
}
#endif

void BIODispatcher::AddUnit(BIOExecutionUnit* pUnit)
{
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);
	pUnit->SetDispatcher(this);
	return pIntern->AddUnit(pUnit);
}

BMemoryShack_char* BIODispatcher::GetShack() 
{ 
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);
	return pIntern->GetShack();
}

void BIODispatcher::PushWork(BIOContext* pContext)
{
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);
	pIntern->PushWork(pContext);
}

BIOContext* BIODispatcher::PopWork()
{
	I_IODispatcher* pIntern = reinterpret_cast<I_IODispatcher*>(m_pIntern);
	return pIntern->PopWork();
}
