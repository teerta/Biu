
#pragma once


#include "bcp.h"


using namespace System;
using namespace Biu;


namespace Biu {
	namespace Native {
		namespace Security {


			ref class CryptKey;
			ref class KeyBlob;


			/// <summary>
			/// The foundation (or container) for encryption and decryption.
			/// </summary>
			public ref class CryptProvider sealed
			{
			public:
				CryptProvider();
				~CryptProvider();
				!CryptProvider();

				CryptKey^ __clrcall GenerateAES256Key();

				KeyBlob^ __clrcall GenerateAES256Blob();


				CryptKey^ __clrcall ImportSessionKey(KeyBlob^ blob);

				inline CryptKey^ __clrcall ImportAES256Blob(KeyBlob^ blob)
				{
					return ImportBlobSessiobKey(blob, CALG_SHA_256, CALG_AES_256, 0x01000000);
				}


			internal:
				BCryptProvider* m_pProv;


				CryptKey^ __clrcall ImportBlobSessiobKey(KeyBlob^ blob, ALG_ID algHash, ALG_ID algKey, DWORD dwFlag);
			};


		}
	}
}
