#include "stdafx.h"
#include "SecurityProvider.h"


using namespace Biu::Transmission;


SecurityProvider::SecurityProvider()
	: m_pInner(NULL)
{
	m_pInner = new BSecurityProvider();
}

SecurityProvider::~SecurityProvider()
{
	this->!SecurityProvider();
}

SecurityProvider::!SecurityProvider()
{	
	SAFE_DELETE(m_pInner);
}

bool SecurityProvider::NewItem()
{
	auto* p = m_pInner->NewItem();
	return NULL != p;
}
