
#pragma once


#include "Interface.h"


using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;


namespace Biu {
	namespace Native {
		namespace Security {


			ref class CryptKey;


			/// <summary>
			/// AES algorithm who is based on windows enhanced AES provider.
			/// </summary>
			public ref class AES sealed : public IStreamTransform
			{
			public:
				AES();
				~AES();


				/// <summary>
				/// Encrypt a stream by CryptKey and compute a hash (SHA256).
				/// Handle all remaining data in stream from Stream.Position when size is 0, otherwise encrypt data up to length of size.
				/// </summary>
				virtual void __clrcall EncryptStream(Stream^ src, Stream^ dest, CryptKey^ key, unsigned int size, cli::array<Byte>^% hash);

				/// <summary>
				/// Decrypt a stream by CryptKey and compute a hash (SHA256) for source data (unencrypted data).
				/// Handle all remaining data in stream from Stream.Position when size is 0, otherwise decrypt data up to length of size.
				/// </summary>
				virtual void __clrcall DecryptStream(Stream^ src, Stream^ dest, CryptKey^ key, unsigned int size, IEnumerable<Byte>^ hash);


			private:
				DWORD m_dwBatch;
			};


		}
	}
}
