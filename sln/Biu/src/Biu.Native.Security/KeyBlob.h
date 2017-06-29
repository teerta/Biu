
#pragma once


#include "bcp.h"


using namespace System;
using namespace Biu;
using namespace System::IO;
using namespace System::Collections::Generic;


namespace Biu {
	namespace Native {
		namespace Security {


			ref class CryptKey;


			/// <summary>
			/// A binary key container.
			/// </summary>
			public ref class KeyBlob sealed
			{
			public:
				KeyBlob();
				~KeyBlob();
				!KeyBlob();

				property CryptKey^ EncryptionKey;
				property bool IsEncrypted;


				bool __clrcall Save(String^ path);
				bool __clrcall Save(Stream^ stream);

				cli::array<Byte>^ ToArray();


				static KeyBlob^ __clrcall FromFile(String^ path);
				static KeyBlob^ __clrcall FromBuffer(IEnumerable<Byte>^ coll);


			internal:
				DWORD m_cbKey;
				LPBYTE m_lpKey;


				KeyBlob(DWORD cbKey);
			};


		}
	}
}
