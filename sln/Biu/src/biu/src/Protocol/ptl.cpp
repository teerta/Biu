
#include "stdafx.h"
#include "bptl.h"
#include "bbfunc.h"


using namespace Biu;


// BProtocolRestriction

BProtocolRestriction::BProtocolRestriction()
    : m_ullIdentification(0)
    , m_ulMaxPacket(0)
    , m_ulMinPacket(0)
{
}

BProtocolRestriction::~BProtocolRestriction()
{
}
