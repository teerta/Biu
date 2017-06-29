#include "stdafx.h"
#include "MemoryShack.h"


using namespace Biu::Transmission;


MemoryShack::MemoryShack()
	: m_pShack(NULL)
{
	m_pShack = new BMemoryShack_char();
}

MemoryShack::~MemoryShack()
{
	this->!MemoryShack();
}

MemoryShack::!MemoryShack()
{
	SAFE_DELETE(m_pShack);
}
