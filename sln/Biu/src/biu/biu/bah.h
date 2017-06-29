#ifndef _BAH_H_
#define _BAH_H_


// Biu Quick IO Handler


#include "biu.h"
#include "bs.h"
#include "baqdef.h"
#include "biohand.h"


namespace Biu {


	// Class Ashley_Handler

	class BIU_API Ashley_Handler : public BIONavigatedHandler
	{
	public:
		Ashley_Handler(unsigned long long ullIdentification, unsigned int ulMinPacketSize = 0, unsigned int ulMaxPacketSize = 0);
		virtual ~Ashley_Handler();

		virtual BProtocolRestriction* GetRestriction();

	protected:
		BProtocolRestriction			m_restriction;

		bool ExtractProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation, unsigned long long* pAction);
	};


	// Class Ashley_ChannelHandler

	class BIU_API Ashley_ChannelHandler : public BIOChannelHandler
	{
	public:
		Ashley_ChannelHandler(unsigned long long ullIdentification, unsigned int ulMinPacketSize = 0, unsigned int ulMaxPacketSize = 0);
		virtual ~Ashley_ChannelHandler();

		virtual BProtocolRestriction* GetRestriction();

	protected:
		BProtocolRestriction			m_restriction;

		bool ExtractProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation, unsigned long long* pAction);
	};


}


#endif // _BAH_H_
