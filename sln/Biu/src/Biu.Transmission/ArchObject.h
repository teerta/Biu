
#pragma once


#include "Serializer.h"


using namespace System;


namespace Biu{ namespace Transmission {


	// Ref Class BiuDataHeader

	public ref class BiuDataHeader : public IBiuSerializable
	{
	public:
		__clrcall BiuDataHeader();
		__clrcall ~BiuDataHeader();

		virtual bool __clrcall BSerialize(BiuSerializer^ ser);
		virtual bool __clrcall BDeserialize(BiuSerializer^ ser);


		long long					m_ID;
		unsigned long long			m_Total;
		unsigned long long			m_Current;
		unsigned int				m_BufferSize;
	};


} }
