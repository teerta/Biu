
#pragma once


namespace Biu { namespace Transmission {


		// Class TransmissionManager

		public ref class TransmissionManager
		{
		public:
			static bool __clrcall StartUpWSA();
			static bool __clrcall CleanUpWSA();
		};


} }
