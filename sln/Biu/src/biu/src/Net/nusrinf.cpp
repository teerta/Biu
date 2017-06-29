
#include "stdafx.h"
#include "bnctxt.h"
#include "bbfunc.h"
#include <cassert>


using namespace std;
using namespace Biu;


// BNUserInfo

BNUserInfo::BNUserInfo()
	: m_nUID(0)
	, m_uiPlatform(0)
{
}

BNUserInfo::~BNUserInfo()
{
}

void BNUserInfo::SetUserName(LPCTSTR lpsz)
{
	m_strUserName = lpsz;
}

void BNUserInfo::GetUserName(CString& str)
{
	str = m_strPassword;
}

void BNUserInfo::SetPassword(LPCTSTR lpsz)
{
	m_strPassword = lpsz;
}

void BNUserInfo::GetPassword(CString& str)
{
	str = m_strPassword;
}
