
#pragma once


using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;


namespace Biu {
	namespace Native {
		namespace Security {


			ref class CryptKey;


			public interface class IStreamTransform
			{
			public:
				void __clrcall EncryptStream(Stream^ src, Stream^ dest, CryptKey^ key, unsigned int size, cli::array<Byte>^% hash);
				void __clrcall DecryptStream(Stream^ src, Stream^ dest, CryptKey^ key, unsigned int size, IEnumerable<Byte>^ hash);
			};


		}
	}
}
