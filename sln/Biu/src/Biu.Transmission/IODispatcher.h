#pragma once


#include "biodisp.h"
#include "bmshack.h"


using namespace Biu;


namespace Biu { namespace Transmission { 


	// Forwarding

	ref class MemoryShack;
	ref class NetIOExecutionUnit;


	// Ref Class IODispatcher

	public ref class IODispatcher
	{
	public:
		__clrcall IODispatcher(MemoryShack^ shack, unsigned int numberOfIOs, unsigned int numberOfWorkers);
		__clrcall ~IODispatcher();
		__clrcall !IODispatcher();

		void __clrcall AddUnit(NetIOExecutionUnit^ unit);


	internal:
		property BIODispatcher* Dispatcher
		{
			BIODispatcher* get()
			{
				return m_pDispatcher;
			}
		}


	private:
		BIODispatcher*				m_pDispatcher;
	};


} }
