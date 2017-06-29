#pragma once


#include "bsec.h"


using namespace System;
using namespace Biu;


namespace Biu { namespace Transmission {


	// Ref Class SecurityProvider

	public ref class SecurityProvider
	{
	public:
		SecurityProvider();
		~SecurityProvider();
		!SecurityProvider();

		bool __clrcall NewItem();


	internal:
		BSecurityProvider* m_pInner;
	};


} }
