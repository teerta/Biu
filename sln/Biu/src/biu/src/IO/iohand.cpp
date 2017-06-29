
#include "stdafx.h"
#include "biohand.h"
#include <cassert>


using namespace Biu;


// BIOHandler

BIOHandler::BIOHandler()
{
}

BIOHandler::~BIOHandler()
{
}


// BIONavigatedHandler

BIONavigatedHandler::BIONavigatedHandler()
	: m_channel(NULL)
	, m_ioHandler(NULL)
	, m_cmdHandler(NULL)
{
}

BIONavigatedHandler::~BIONavigatedHandler()
{
}

bool BIONavigatedHandler::HandleProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation)
{
	assert(m_channel);
	assert(m_ioHandler);

	unsigned long long action;
	CHK_false(ExtractProtocol(pDisp, pOperation, &action));

	return m_ioHandler(m_channel, pDisp, action, pOperation);
}

bool BIONavigatedHandler::HandleCommand(BIODispatchable* pDisp, unsigned long long cmd, void* p1, void* p2)
{
	assert(cmd);

	return m_cmdHandler(m_channel, pDisp, cmd, p1, p2);
}


// BIOChannelHandler

BIOChannelHandler::BIOChannelHandler()
	: m_channel(NULL)
	, m_ioHandler(NULL)
	, m_cmdHandler(NULL)
{
}

BIOChannelHandler::~BIOChannelHandler()
{
}

bool BIOChannelHandler::HandleProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation)
{
	assert(m_channel);
	assert(m_ioHandler);

	unsigned long long action;
	CHK_false(ExtractProtocol(pDisp, pOperation, &action));

	return (m_channel->*m_ioHandler)(pDisp, action, pOperation);
}

bool BIOChannelHandler::HandleCommand(BIODispatchable* pDisp, unsigned long long cmd, void* p1, void* p2)
{
	assert(cmd);

	return (m_channel->*m_cmdHandler)(pDisp, cmd, p1, p2);
}
