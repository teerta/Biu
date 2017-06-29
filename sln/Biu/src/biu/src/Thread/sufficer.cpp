
#include "stdafx.h"
#include "bsyncprimx.h"


using namespace Biu;


// BCountedSufficer

BCountedSufficer::BCountedSufficer(int nMax, int nCurrent)
	: m_max(nMax)
	, m_current(nCurrent)
{
}

BCountedSufficer::~BCountedSufficer()
{
}

int BCountedSufficer::IncMax() 
{ 
	m_max++;
	Suffice();

	return m_max;
}

int BCountedSufficer::IncMax(int nCount) 
{
	m_max += nCount;
	Suffice();

	return m_max; 
}

int BCountedSufficer::DecMax() 
{ 
	m_max--;
	Suffice();

	return m_max;
}

void BCountedSufficer::SetMax(int nMax) 
{ 
	m_max = nMax;
	Suffice();
}

void BCountedSufficer::Take()
{
	m_current--;
	Suffice();
}

void BCountedSufficer::Give()
{
	m_current++;
	Suffice();
}

void BCountedSufficer::Clear()
{
	m_max = 0;
	m_current = 0;
	m_evt.Set();
}

void BCountedSufficer::Suffice()
{
//	int nMax = m_max;
//	if (m_current == nMax)
	if (m_max == m_current)
	{
		if (!TryWait())
		{
			m_evt.Set();
		}
	}
	else 
	{
		if (TryWait())
		{
			m_evt.Reset();
		}
	}
}

#ifdef _WINDOWS
bool BCountedSufficer::Wait(unsigned long ulMillisenonds)
{
	return m_evt.Wait(ulMillisenonds);
}

bool BCountedSufficer::TryWait()
{
	return m_evt.TryWait();
}

const int BCountedSufficer::GetMax()
{ 
	return m_max;
}

const int BCountedSufficer::GetCurrent()
{
	return m_current;
}

bool BCountedSufficer::IsEqual()
{
	return m_max == m_current;
}
#endif // _WINDOWS
