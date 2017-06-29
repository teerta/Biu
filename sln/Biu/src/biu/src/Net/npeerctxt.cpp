
#include "stdafx.h"
#include "bnctxt.h"
#include "bbfunc.h"
#include <cassert>
#include "bfunc.h"


using namespace std;
using namespace Biu;


// BNPeerContext

BNPeerContext::BNPeerContext()
	: m_pRepresentation(NULL)
{
	m_dwNumberOfInvalidCmd = 0;
	m_fIsLogin = false;
	m_dwPlatform = 0;
	m_nLastHeartBeating = 0;
}

BNPeerContext::~BNPeerContext()
{
}
