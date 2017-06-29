
#include "stdafx.h"
#include "bah.h"
#include "bnio.h"
#include "bashley.h"


using namespace Biu;


// Ashley_Handler

Ashley_Handler::Ashley_Handler(unsigned long long ullIdentification, unsigned int ulMinPacketSize, unsigned int ulMaxPacketSize)
{
	m_restriction.SetIdnetification(ullIdentification);
	m_restriction.SetMinPacketSize(ulMinPacketSize);
	m_restriction.SetMaxPacketSize(ulMaxPacketSize);
}

Ashley_Handler::~Ashley_Handler()
{
}

bool Ashley_Handler::ExtractProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation, unsigned long long* pAction)
{
	assert(pDisp);

	BPeer* peer = static_cast<BPeer*>(pDisp);

	BAshley* p = static_cast<BAshley*>(peer->GetProtocol());
	assert(p);

	switch (*pOperation)
	{
	case BIO_Accept:
		*pAction = Biu_A_Accept;
		break;
	case BIO_Connect:
		*pAction = Biu_A_Connect;
		break;
	case BIO_Disconnect:
		*pAction = Biu_A_Disconnect;
		break;
	case BIO_Read:
		*pAction = p->GetProcessing();
		break;
		//case BIO_Write:
		//	dwType = p->GetSendHeader()->Type;
		//	break;
	default:
		// Unhandled io
		return false;
		break;
	}

	*pOperation = BIO_None;

	return true;
}

BProtocolRestriction* Ashley_Handler::GetRestriction()
{
	return &m_restriction;
}


// Ashley_ChannelHandler

Ashley_ChannelHandler::Ashley_ChannelHandler(unsigned long long ullIdentification, unsigned int ulMinPacketSize, unsigned int ulMaxPacketSize)
{
	m_restriction.SetIdnetification(ullIdentification);
	m_restriction.SetMinPacketSize(ulMinPacketSize);
	m_restriction.SetMaxPacketSize(ulMaxPacketSize);
}

Ashley_ChannelHandler::~Ashley_ChannelHandler()
{
}

bool Ashley_ChannelHandler::ExtractProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation, unsigned long long* pAction)
{
	assert(pDisp);

	BPeer* peer = static_cast<BPeer*>(pDisp);

	BAshley* p = static_cast<BAshley*>(peer->GetProtocol());
	assert(p);

	switch (*pOperation)
	{
	case BIO_Accept:
		*pAction = Biu_A_Accept;
		break;
	case BIO_Connect:
		*pAction = Biu_A_Connect;
		break;
	case BIO_Disconnect:
		*pAction = Biu_A_Disconnect;
		break;
	case BIO_Read:
		*pAction = p->GetProcessing();
		break;
		//case BIO_Write:
		//	dwType = p->GetSendHeader()->Type;
		//	break;
	default:
		// Unhandled io
		return false;
		break;
	}

	*pOperation = BIO_None;

	return true;
}

BProtocolRestriction* Ashley_ChannelHandler::GetRestriction()
{
	return &m_restriction;
}
