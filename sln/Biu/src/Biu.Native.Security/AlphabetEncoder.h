
#pragma once


#include "bencoder.h"


using namespace Biu;
using namespace System;


namespace Biu { namespace Native { namespace Security {


		// Ref Class AlphabetEncoder

		/// <summary>
		/// A directory based encoder that can encode data to string (from directory).
		/// Characters in directory must be unique for each other.
		/// </summary>
		public ref class AlphabetEncoder sealed
		{
		public:
			__clrcall AlphabetEncoder();
			__clrcall ~AlphabetEncoder();

			String^ __clrcall EncodeString(String^ str);
			String^ __clrcall DecodeString(String^ str);

			void __clrcall SetAlphabet(String^ str);
			void __clrcall ResetAlphabet();

		private:
			BAlphabetEncoder* m_pEnc;
		};


} } }
