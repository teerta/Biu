
#include "stdafx.h"
#include "bsyncprim.h"


using namespace Biu;


//// XUniSiSupplyChain
//
//XUniSiSupplyChain::XUniSiSupplyChain()
//	: m_hevtDone(NULL)
//	, m_hevtWorking(NULL)
//{
//	m_hevtDone = CreateEvent(NULL, TRUE, FALSE, NULL);
//	m_hevtWorking = CreateEvent(NULL, FALSE, FALSE, NULL);
//
//	m_waits[0] = m_hevtDone;
//	m_waits[1] = m_hevtWorking;
//}
//
//XUniSiSupplyChain::~XUniSiSupplyChain()
//{
//	SAFE_CLOSE_HANDLE_NULL(m_hevtDone);
//	SAFE_CLOSE_HANDLE_NULL(m_hevtWorking);
//}
