
#ifndef _BIUHAND_H_
#define _BIUHAND_H_


#include "biu.h"
#include "biodef.h"
#include "bptl.h"


namespace Biu {


	// Forwarding

	class BIODispatchable;
	struct IBHandlerChannel;



	// Type Definitions

	typedef bool(*BProtocolIOHandler)(
		IBHandlerChannel* pChannel,
		BIODispatchable* pDisp,
		unsigned long long action,
		LPB_IO_Operation pOperation
		);

	typedef bool(*BProtocolCommandHandler)(
		IBHandlerChannel* pChannel,
		BIODispatchable* pDisp,
		unsigned long long cmd,
		void* p1,
		void* p2
		);

	typedef bool(IBHandlerChannel::*BChannelIOHandler)(
		BIODispatchable* pDisp,
		unsigned long long action,
		LPB_IO_Operation pOperation
		);

	typedef bool(IBHandlerChannel::*BChannelCommandHandler)(
		BIODispatchable* pDisp,
		unsigned long long cmd,
		void* p1,
		void* p2
		);



	// Interface IBHandlerChannel

	struct IBHandlerChannel
	{
	};



	// Class BIOHandler

	class BIU_API BIOHandler
	{
	public:
		BIOHandler();
		virtual ~BIOHandler();

		/*
		* *pOperation:
		*      BIO_None - Clear all reamining data
		*      BIO_Retire - Will close (when remaining data is gone)
		*      BIO_Close - Close now
		*      BIO_Read - Continue reading (no return data)
		*      BIO_Continue - Send returned data & Read
		*/
		virtual bool HandleProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation) = 0;

		/*
		* cmd:
		*      B_IOHC_NEW_SOCKET - a new socket has been created
		*              p1 - socket (int)
		*              p2 - protocol (IDataChannel*)
		*
		*      B_IOHC_WRITE_DONE - a sending has finished
		*              p1 - number of bytes sent (ssize_t)
		*              p2 - NULL
		*
		*      B_IOHC_CONNECTED - connect ok
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_ACCEPTED - accept ok
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_CLOSE_PROACTIVE - the peer has been closed proactively (local-close-first)
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_CLOSE_PASSIVE - the peer has been closed passively (remote-close-first)
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_CONNECT_REFUSED - the connection has been refused by server
		*              p1 - NULL
		*              p2 - NULl
		*
		*      B_IOHC_NETNAME_UNAVAILABLE - (for windows) the connection is closed
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_HOST_UNAVAILABLE - remote host is unavailable
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_CONNECTION_RESET - connection has been reset by remote host
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_NET_UNAVAILABLE - local network is unavailable
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_CONNECT_TIMEOUT - a connecting peer is time-out
		*              p1 - NULL
		*              p2 - NULL
		*
		*      B_IOHC_ERROR - other error
		*			   p1 - Calling Site (unsigned long long) (See 'Errors for Calling Sites' in berr.h)
		*              p2 - errno (ssize_t)
		*
		*/
		virtual bool HandleCommand(BIODispatchable* pDisp, unsigned long long cmd, void* p1, void* p2) = 0;
		virtual BProtocolRestriction* GetRestriction() = 0;
	};


	// Class BIONavigatedHandler

	class BIU_API BIONavigatedHandler : public BIOHandler
	{
	public:
		BIONavigatedHandler();
		virtual ~BIONavigatedHandler();

		virtual bool HandleProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation);
		virtual bool HandleCommand(BIODispatchable* pDisp, unsigned long long cmd, void* p1, void* p2);

		inline void SetHandler(IBHandlerChannel* pChannel, BProtocolIOHandler pIo, BProtocolCommandHandler pCmd)
		{
			m_channel = pChannel;
			m_ioHandler = pIo;
			m_cmdHandler = pCmd;
		}

	protected:
		virtual bool ExtractProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation, unsigned long long* pAction) = 0;

		IBHandlerChannel* m_channel;
		BProtocolIOHandler m_ioHandler;
		BProtocolCommandHandler m_cmdHandler;
	};


	// Class BIOChannelHandler

	class BIU_API BIOChannelHandler : public BIOHandler
	{
	public:
		BIOChannelHandler();
		virtual ~BIOChannelHandler();

		virtual bool HandleProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation);
		virtual bool HandleCommand(BIODispatchable* pDisp, unsigned long long cmd, void* p1, void* p2);

		inline void SetHandler(IBHandlerChannel* pChannel, BChannelIOHandler pIo, BChannelCommandHandler pCmd)
		{
			m_channel = pChannel;
			m_ioHandler = pIo;
			m_cmdHandler = pCmd;
		}

	protected:
		virtual bool ExtractProtocol(BIODispatchable* pDisp, LPB_IO_Operation pOperation, unsigned long long* pAction) = 0;

		IBHandlerChannel* m_channel;
		BChannelIOHandler m_ioHandler;
		BChannelCommandHandler m_cmdHandler;
	};


}


#endif // _BIUHAND_H_
