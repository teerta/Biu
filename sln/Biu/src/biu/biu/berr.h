
#ifndef _BERR_H_
#define _BERR_H_


namespace Biu {


	// Errors for Calling Sites


	// Enumeration BCallingSite

	enum BCallingSite : unsigned long long
	{
		BCS_None = 0,

		BCS_NIoExec_Connect_BeginConnect = 0x0000000000100000,
		BCS_NIoExecIo_ExecuteIo_UnhandledIoErr = 0x0000000000100001,
		BCS_PeerI_Close_BeginDisconnect = 0x0000000000100002,
		BCS_PeerI_Craft_BeginWrite = 0x0000000000100003,
		BCS_PeerI_Craft_BeginRead = 0x0000000000100004,
		BCS_PeerI_ActAsClient_Bing = 0x0000000000100005,
		BCS_PeerI_EndRead_BeginRead = 0x0000000000100006,
		BCS_PeerI_EndWrite_BeginWrite = 0x0000000000100007,
		BCS_PeerI_EndAccept_UpdateContext = 0x0000000000100008,
		BCS_PeerI_EndAccept_JoinTheCircle = 0x0000000000100009,
		BCS_PeerI_EndConnect_UpdateContext = 0x0000000000100001,
		BCS_PeerI_EndConnect_BeginRead = 0x000000000010000b,
		BCS_PeerI_EndTransFile_BeginTransFile = 0x000000000010000c,
		BCS_PeerI_Send_BeginWrite = 0x000000000010000d,
		BCS_PeerI_OutterSend_BeginWrite = 0x000000000010000e,
		BCS_PeerI_SendPod_BeginWrite = 0x000000000010000f,
		BCS_PeerI_ClientSendFile_BeginTransFile = 0x0000000000100010,
		BCS_PeerI_ClientSendFileFi_BeginTransFile = 0x0000000000100011,
		BCS_PeerI_ServerSendFile_BeginTransFile = 0x0000000000100012,
		BCS_PeerI_EndAccept_BeginRead = 0x0000000000100013,
		BCS_NIoExec_PostAccept_CreateAccept = 0x0000000000100014,
		BCS_NIoExec_PostAccept_BeginAccept = 0x0000000000100015,

		BCS_Unknown = 0xffffffffffffffff
	};


}


#endif // _BERR_H_
