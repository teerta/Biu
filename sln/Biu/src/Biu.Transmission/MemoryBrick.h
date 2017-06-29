#pragma once


#include "bmshack.h"


using namespace Biu;


namespace Biu{ namespace Transmission {


	// Ref Class MemoryBrick

	public ref class MemoryBrick
	{
	public:
		MemoryBrick();
		~MemoryBrick();

	internal:
		property BMemoryBrick* Brick
		{
			BMemoryBrick* get() { return m_pBrick; }
			void set(BMemoryBrick* value) { m_pBrick = value; }
		}

	private:
		BMemoryBrick* m_pBrick;
	};


} }
