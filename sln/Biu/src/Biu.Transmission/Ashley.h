
#pragma once


#include "bashley.h"
#include "Serializer.h"
#include "MemoryBrick.h"
#include "IOSecurityContext.h"


using namespace Biu;


namespace Biu { namespace Transmission {


		// Enum Class AshleyVersion

		public enum class AshleyVersion : unsigned int
		{
			V1				= 0x00010000,
			V1_1			= 0x00010100
		};


		// Enum Class AshleyFlag

		[FlagsAttribute]
		public enum class AshleyFlag : unsigned int
		{
			None			= 0,

			Recog			= 0x00000001,
			Ack				= 0x00000002,
			Verification	= 0x00000004,
			Security		= 0x00000008,
			Part			= 0x00000010,
			Displacement	= 0x00000020
		};

		public enum class AshleySecurityAlg : unsigned short
		{
			None			= 0,

			P4_1			= 0x0001,
			P4S				= 0x0002
		};


		// Ref Class AshleyHeader_Base

		public ref class AshleyHeader_Base
		{
		public:
			__clrcall AshleyHeader_Base()
			{
			}

			__clrcall ~AshleyHeader_Base()
			{
			}

			property AshleyVersion Version;
			property unsigned int Flag;			
		};


		// Ref Class AshleyHeader_Security

		public ref class AshleyHeader_Security
		{
		public:
			__clrcall AshleyHeader_Security()
			{
			}

			__clrcall ~AshleyHeader_Security()
			{
			}

			property AshleySecurityAlg Alg;			
		};


		// Ref Class AshleyHeader_Recog

		public ref class AshleyHeader_Recog
		{
		public:
			__clrcall AshleyHeader_Recog()
			{
			}

			__clrcall ~AshleyHeader_Recog()
			{
			}

			property unsigned long long Processing;
			property unsigned long long Identification;
		};


		// Ref Class AshleyHeader_Part

		public ref class AshleyHeader_Part
		{
		public:
			__clrcall AshleyHeader_Part()
			{
			}

			__clrcall ~AshleyHeader_Part()
			{
			}

			property long long ID;
			property unsigned long long Total;
			property unsigned long long Current;
		};


		// Ref Class AshleyHeader

		public ref class AshleyHeader
		{
		public:
			__clrcall AshleyHeader();
			__clrcall ~AshleyHeader();

			property AshleyHeader_Base^ Base;
			property AshleyHeader_Security^ Security;
			property AshleyHeader_Recog^ Recog;
			property AshleyHeader_Part^ Part;

		internal:
			void ToNativeHeader(LPB_ASHLEY_HEADER lpHeader);
			void FromNativeHeader(LPB_ASHLEY_HEADER lpHeader);
		};


		// Ref Class Ashley

		public ref class Ashley sealed
		{
		public:
			Ashley();
			~Ashley();
			!Ashley();


			bool __clrcall GetObject(IBiuSerializable^ obj);
			bool __clrcall CreateObject(MemoryBrick^ brick, IBiuSerializable^ obj, AshleyHeader^ header, IOSecurityContext^ sec);
			IBiuSerializable^ __clrcall KillObject();

			generic<class T>
			where T : IBiuSerializable
			bool __clrcall GetObjKept(T% obj);

			int __clrcall GetReceivedRaw(MemoryBrick^% brick, unsigned long long% pos);


			static Ashley^ __clrcall CreateEntityProtocol();
			static AshleyHeader^ __clrcall CreateHeader(AshleyVersion version, unsigned long long identification,
				unsigned long long processing, AshleyFlag flag);


		internal:
			property BAshley* AshleyProtocol
			{
				void set(BAshley* pAshley)
				{
					m_pAshley = pAshley;

					m_sr = gcnew BiuSerializer();
					m_sr->Serializer = m_pAshley->GetRecvSerializer();

					m_ss = gcnew BiuSerializer();
					m_ss->Serializer = m_pAshley->GetSendSerializer();
				}

				BAshley* get()
				{
					return m_pAshley;
				}
			}

			bool m_bEntity;

		private:
			BAshley* m_pAshley;
			BiuSerializer^ m_sr;
			BiuSerializer^ m_ss;
			IBiuSerializable^ m_obj;
		};


} }
