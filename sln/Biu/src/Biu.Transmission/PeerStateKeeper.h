
#pragma once


#include "Serializer.h"
#include "Peer.h"


using namespace System;


namespace Biu {
	namespace Transmission {

		
		public ref class PeerStateKeeper
		{
		public:
			__clrcall PeerStateKeeper(Peer^ peer);
			__clrcall ~PeerStateKeeper();
			__clrcall !PeerStateKeeper();

			
			void __clrcall Restore();


		internal:
			Peer^ m_peer;


		private:
			BMemoryBrick* m_pBrick;
		};


	}
}
