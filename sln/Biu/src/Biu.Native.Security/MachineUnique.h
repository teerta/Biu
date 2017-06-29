
#pragma once


#include "bencoder.h"


using namespace Biu;
using namespace System;


namespace Biu { namespace Native { namespace Security {
			

	/// <summary>
	/// Generate unique string from hardwares.
	/// </summary>
	public ref class MachineUnique
	{
	public:
		__clrcall MachineUnique();

		/// <summary>
		/// For administrators.
		/// </summary>
		static String^ __clrcall DefaultHardwareID();

		/// <summary>
		/// For users.
		/// </summary>
		static String^ __clrcall DefaultHardwareIDForUser();

		
	private:
		static const int Max_ID_Size = 1024;
	};


} } }
