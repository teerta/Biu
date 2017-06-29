
#ifndef _BAQDEF_H_
#define _BAQDEF_H_


#define BIU_ACTION_BASE		0x00000100
#define BIU_ACK_BASE		0x00010000


typedef enum tagBIU_ACTION : unsigned long long {
	Biu_A_None = 0x00000000,

	Biu_A_Accept = 0x00000001,
	Biu_A_Connect = 0x00000002,
	Biu_A_Disconnect = 0x00000003,

	Biu_A_Hello = 0x00000010,

	Biu_A_Ack = 0x00000030,
	Biu_A_AckMsg = 0x00000031,
	Biu_A_Heart_Beating = 0x00000032,

	Biu_A_Login = 0x00000040,
	Biu_A_LoginToken = 0x00000041,
	Biu_A_ChangePassword = 0x00000042,
	Biu_A_ChangeUserPassword = 0x00000043,

	Biu_A_Request = 0x00000052,
	Biu_A_Response = 0x00000053,

	Biu_A_PublicKey = 0x00000054,
	Biu_A_PrivateKey = 0x00000055,
	Biu_A_SecurityEvidence = 0x00000056,
	Biu_A_SecurityToken = 0x00000057,
	Biu_A_SecurityIdentity = 0x00000058,

	Biu_A_FileData = 0x000000a0,
	Biu_A_SendFile = 0x000000a1,
	Biu_A_RecvFile = 0x000000a2,
	Biu_A_AskFile = 0x000000a3,
	Biu_A_ReplyFile = 0x000000a4,
	Biu_A_PurifyFile = 0x000000a5
} BIU_ACTION, *LPBIU_ACTION;


typedef enum tagBIU_ACKRESULT : unsigned int
{
	Biu_Ack_None = 0x00000000,

	Biu_Ack_OK = 0x00000001,
	Biu_Ack_Failed = 0x00000002,
	Biu_Ack_PermissionDenied = 0x00000003,
	Biu_Ack_NotImplemented = 0x00000004,
	Biu_Ack_Unsupported = 0x00000005,
	Biu_Ack_Pending = 0x00000006,
	Biu_Ack_Unavailable = 0x00000007,

	Biu_Ack_NoPublicKey = 0x00000010,
	Biu_Ack_NoPrivateKey = 0x00000011,
	Biu_Ack_InvalidPublicKey = 0x00000012,
	Biu_Ack_InvalidPrivateKey = 0x00000013,
	Biu_Ack_InvalidSecurityEvidence = 0x00000014,
	Biu_Ack_InvalidSecurityToken = 0x00000015,
	BIu_Ack_InvalidSecurityIdentity = 0x00000016,

	Biu_Ack_AccountBlocked = 0x00000100,
	Biu_Ack_SameUserLogin = 0x00000101,
	Biu_Ack_MaxSameUserLogin = 0x00000102,
	Biu_Ack_InvalidUser = 0x00000104,
	Biu_Ack_InvalidPassword = 0x00000105,
	Biu_Ack_InvalidToken = 0x00000106,

	Biu_Ack_InvalidOperation = 0x00000120,
	Biu_Ack_Exist = 0x00000121,
	Biu_Ack_NotFound = 0x00000122,
	Biu_Ack_MaxQuota = 0x00000123,
	Biu_Ack_MaxAccess = 0x00000124,
	Biu_Ack_MaxRead = 0x00000125,
	Biu_Ack_MaxWrite = 0x00000126,
	Biu_Ack_Occupied = 0x00000127,

	Biu_Ack_Free = 0x000001a0,
	Biu_Ack_Idle = 0x000001a1,
	Biu_Ack_Working = 0x000001a2,
	Biu_Ack_Running = 0x000001a3,
	Biu_Ack_Paused = 0x000001a4,
	Biu_Ack_Stopped = 0x000001a5,

	Biu_Ack_CreateFileFailed = 0x00000200,
	Biu_Ack_MaxRecvFile = 0x00000201,
	Biu_Ack_MaxSendFile = 0x00000202,

	Biu_Ack_Underlying_Service_Fault = 0x00001000,
	Biu_Ack_Server_Fault = 0x00001001,
	Biu_Ack_System_Fault = 0x00001002
} BIU_ACKRESULT, *LPBIU_ACKRESULT;


typedef enum tagBIU_OPMODE : int {
	Biu_OPM_None = 0x00000000,

	Biu_OPM_Read = 0x00000100,
	Biu_OPM_Write = 0x00000101,
	Biu_OPM_Seek = 0x00000102,
	Biu_OPM_Delete = 0x00000103,
	Biu_OPM_Rename = 0x00000104,
	Biu_OPM_Move = 0x00000105,
	Biu_OPM_Hash = 0x00000106,
	Biu_OPM_Attribute = 0x00000107,
	Biu_OPM_ChangeAttribute = 0x00000108,
	Biu_OPM_Compress = 0x00000109,
	Biu_OPM_Decompress = 0x00000110,
	Biu_OPM_DupNameDeny = 0x00000111,
	Biu_OPM_DupNameRandom = 0x00000112,
	Biu_OPM_DupNameSeq = 0x00000113,
	Biu_OPM_DupNameOverwrite = 0x00000114,
	Biu_OPM_DupNameOverwriteBackup = 0x00000115,
	Biu_OPM_DupNameSkip = 0x00000116,
	Biu_OPM_DupNamePatternRandom = 0x00000117
} BIU_OPMODE, *LPBIU_OPMODE;


#endif // _BAQDEF_H_
