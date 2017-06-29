#pragma once


#include "biosecc.h"
#include "SecurityProvider.h"


using namespace System;
using namespace Biu;


namespace Biu { namespace Transmission {


	// Ref class IOSecurityContext

	public ref class IOSecurityContext
	{
	public:
		IOSecurityContext();
		~IOSecurityContext();
		!IOSecurityContext();

		bool __clrcall ChangeRemoateKey(cli::array<byte>^ rawKey);
		cli::array<byte>^ __clrcall ExportKey();


	internal:
		BIOSecurityContext*  m_pInner;
	};


}}
