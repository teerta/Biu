
#pragma once


#include "biohand.h"


using namespace Biu;


namespace Biu { namespace Transmission {


	// Class NHandlerChannel

	class NHandlerChannel : public IBHandlerChannel
	{
	public:
		NHandlerChannel();
		~NHandlerChannel();


		bool HandleIo(
			BIODispatchable* pDisp,
			unsigned long long action,
			LPB_IO_Operation pOperation
			);

		bool HandleCmd(
			BIODispatchable* pDisp,
			unsigned long long cmd,
			void* p1,
			void* p2
			);


		void*						m_pHandler;
	};


}}
