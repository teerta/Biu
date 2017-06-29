#pragma once


#include "bmshack.h"


using namespace Biu;


namespace Biu { namespace Transmission {


	public ref class MemoryShack
	{
	public:
		__clrcall MemoryShack();
		__clrcall ~MemoryShack();
		__clrcall !MemoryShack();


	internal:
		BMemoryShack_char* m_pShack;
	};


}}
