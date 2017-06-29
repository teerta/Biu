
#pragma once


#include "bcp.h"


using namespace System;
using namespace Biu;


namespace Biu {
	namespace Native {
		namespace Security {


			ref class KeyBlob;


			/// <summary>
			/// A key for encryption or decryption.
			/// </summary>
			public ref class CryptKey sealed
			{
			public:
				CryptKey();
				~CryptKey();
				!CryptKey();


				property String^ Name;
				property bool IsVital;


				KeyBlob^ __clrcall ExportSessionKey(CryptKey^ encKey, bool keepKeyForBlob);


			internal:
				BCryptProvider* m_pProv;
				HCRYPTKEY m_hKey;
			};


		}
	}
}
