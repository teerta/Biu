
#include "stdafx.h"
#include "bnctxt.h"
#include "bbfunc.h"
#include <cassert>


using namespace std;
using namespace Biu;


// BNClientContext

BNClientContext::BNClientContext()
	: m_pContextFactory(NULL)
	, m_pRepresentation(NULL)
	, m_pPeerContext(NULL)
	, m_pUserContext(NULL)
	, m_pUserStat(NULL)
{

}

BNClientContext::~BNClientContext()
{
	SAFE_DELETE(m_pPeerContext);
	SAFE_DELETE(m_pUserContext);
	SAFE_DELETE(m_pUserStat);
}

void BNClientContext::Create(BPeerRepresentation* pRepresentation, BNUserInfo& userInfo)
{
	assert(pRepresentation);

	m_pRepresentation = pRepresentation;
	m_pPeerContext = m_pContextFactory->CreatePeerContext();
	m_pUserContext = m_pContextFactory->CreateUserContext();
	m_pUserStat = m_pContextFactory->CreateUserStatistics();

	m_pUserContext->SetFromUserInfo(userInfo);
}
