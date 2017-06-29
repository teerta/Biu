
#include "stdafx.h"
#include "bsyncprim.h"


using namespace Biu;


// BScopeLock

BScopeLock::BScopeLock(IBScopeLockable* sl)
	: m_sl(NULL)
{
	if (!sl)
	{
		throw false;
	}

	m_sl = sl;

	m_sl->ScopeLock();
}

BScopeLock::~BScopeLock()
{
	if (m_sl)
	{
		m_sl->ScopeUnlock();
	}
}
