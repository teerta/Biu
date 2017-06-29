
#include "stdafx.h"
#include "bnctxt.h"
#include "bbfunc.h"
#include <cassert>


using namespace std;
using namespace Biu;


// BNUserContext

BNUserContext::BNUserContext()
	: m_lUID(0)
{
	m_lRefCount = 0;
}

BNUserContext::~BNUserContext()
{
}

void BNUserContext::SetFromUserInfo(BNUserInfo& userInfo)
{
	SetUID(userInfo.GetUID());

	CString str;
	userInfo.GetUserName(str);
	SetUserName(str);
}

void BNUserContext::GetUserName(CString& str)
{
	str = m_strUserName;
}

void BNUserContext::SetUserName(LPCTSTR lpsz)
{
	m_strUserName = lpsz;
}
