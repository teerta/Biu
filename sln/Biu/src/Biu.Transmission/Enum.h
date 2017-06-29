
#pragma once


namespace Biu { namespace Transmission {


		// Enum Class IOOperation

		public enum class IOOperation : unsigned int
		{
			None							= 0x00000000,

			Accept							= 0x00000001,
			Close							= 0x00000002,
			Connect							= 0x00000004,
			Disconnect						= 0x00000008,
			Read							= 0x00000010,
			Write							= 0x00000020,
			Retire							= 0x00000040,
			Continue						= 0x00000080,
			Abort							= 0x00000100,
			File							= 0x00000200,
			Packet							= 0x00000400,

			Error							= 0x00100000
		};


		// Enum Class PeerTransmitResult
		
		public enum class PeerTransmitResult : unsigned int
		{
			True = 0,
			False,
			Closing,
			Wait,
			Finish,
			Pending
		};


		// Enum Class SocketType

		public enum class SocketType
		{
			IPv4TCP = 1,
			IPv4UDP,
			IPv6TCP = 20,
			IPv6UDP,
			UnixDomainTCP = 50,
			UnixDomainUDP,
			Raw = 1000
		};


		// Enum Class IOReadMode

		public enum class IOReadMode
		{
			Serial = 1,
			Parallel
		};


		// Enum Class BiuAction

		public enum class BiuAction : unsigned long long 
		{
			None = 0x00000000,

			Accept = 0x00000001,
			Connect = 0x00000002,
			Disconnect = 0x00000003,

			Hello = 0x00000010,

			Ack = 0x00000030,
			AckMsg = 0x00000031,
			HeartBeating = 0x00000032,

			Login = 0x00000040,
			LoginToken = 0x00000041,
			ChangePassword = 0x00000042,
			ChangeUserPassword = 0x00000043,

			PublicKey = 0x00000054,
			PrivateKey = 0x00000055,
			SecurityEvidence = 0x00000056,
			SecurityToken = 0x00000057,
			SecurityIdentity = 0x00000058,

			Request = 0x00000052,
			Response = 0x00000053,

			FileData = 0x000000a0,
			SendFile = 0x000000a1,
			RecvFile = 0x000000a2,
			AskFile = 0x000000a3,
			ReplyFile = 0x000000a4,
			PurifyFile = 0x000000a5
		};


		// Enum Class BiuAckResult

		public enum class BiuAckResult : unsigned int
		{
			None = 0x00000000,

			OK = 0x00000001,
			Failed = 0x00000002,
			PermissionDenied = 0x00000003,
			NotImplemented = 0x00000004,
			Unsupported = 0x00000005,
			Pending = 0x00000006,
			Unavailable = 0x00000007,

			NoPublicKey = 0x00000010,
			NoPrivateKey = 0x00000011,
			InvalidPublicKey = 0x00000012,
			InvalidPrivateKey = 0x00000013,
			InvalidSecurityEvidence = 0x00000014,
			InvalidSecurityToken = 0x00000015,
			InvalidSecurityIdentity = 0x00000016,

			AccountBlocked = 0x00000100,
			SameUserLogin = 0x00000101,
			MaxSameUserLogin = 0x00000102,
			InvalidUser = 0x00000104,
			InvalidPassword = 0x00000105,
			InvalidToken = 0x00000106,

			InvalidOperation = 0x00000120,
			Exist = 0x00000121,
			NotFound = 0x00000122,
			MaxQuota = 0x00000123,
			MaxAccess = 0x00000124,
			MaxRead = 0x00000125,
			MaxWrite = 0x00000126,
			Occupied = 0x00000127,

			Free = 0x000001a0,
			Idle = 0x000001a1,
			Working = 0x000001a2,
			Running = 0x000001a3,
			Paused = 0x000001a4,
			Stopped = 0x000001a5,

			CreateFileFailed = 0x00000200,
			MaxRecvFile = 0x00000201,
			MaxSendFile = 0x00000202,

			UnderlyingServiceFault = 0x00001000,
			ServerFault = 0x00001001,
			SystemFault = 0x00001002
		};


		public enum class SocketTrait
		{
			Normal = 1,
			Data
		};


		public enum class BiuOpMode : int
		{
			None = 0x00000000,

			Read = 0x00000100,
			Write = 0x00000101,
			Seek = 0x00000102,
			Delete = 0x00000103,
			Rename = 0x00000104,
			Move = 0x00000105,
			Hash = 0x00000106,
			Attribute = 0x00000107,
			ChangeAttribute = 0x00000108,
			Compress = 0x00000109,
			Decompress = 0x00000110,
			DupNameDeny = 0x00000111,
			DupNameRandom = 0x00000112,
			DupNameSeq = 0x00000113,
			DupNameOverwrite = 0x00000114,
			DupNameOverwriteBackup = 0x00000115,
			DupNameSkip = 0x00000116,
			DupNamePatternRandom = 0x00000117
		};
}}
