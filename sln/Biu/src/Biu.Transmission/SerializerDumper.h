
#pragma once


#include "Serializer.h"
#include "MemoryShack.h"


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::IO;
using namespace Biu;


namespace Biu { namespace Transmission {


		public ref class BiuSerializationDumper sealed
		{
		public:
			__clrcall BiuSerializationDumper(MemoryShack^ shack);
			__clrcall ~BiuSerializationDumper();
			__clrcall !BiuSerializationDumper();


			generic<class T>
			where T : IBiuSerializable
			bool __clrcall Dump(Stream^ stream, T obj);

			generic<class T>
			where T : IBiuSerializable
			T __clrcall Recycle(Stream^ stream);

		private:
			BiuSerializer^ m_serializer;
			BSerializer* m_pSerializer;
			BMemoryShack_char* m_pShack;
			BMemoryBrick* m_pBrick;
		};


} }
