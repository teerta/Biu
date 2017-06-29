
#ifndef _BIODEF_H_
#define _BIODEF_H_


namespace Biu {


	#define BIO_CMD_NEW_HANDLE				0x00000010
	#define BIO_CMD_ERROR					0x00000011
	#define BIO_CMD_TIMEOUT					0x00000012
	#define BIO_CMD_READ					0x00000013
	#define BIO_CMD_READ_DONE				0x00000014
	#define BIO_CMD_WRITE					0x00000015
	#define BIO_CMD_WRITE_DONE				0x00000016
	#define BIO_CMD_OPEN					0x00000017
	#define BIO_CMD_CLOSE					0x00000018
	#define BIO_CMD_CLOSE_PROACTIVE			0x00000019
	#define BIO_CMD_CLOSE_PASSIVE			0x0000001a
	#define BIO_CMD_CONTROL					0x0000001b
	#define BIO_CMD_PEEK					0x0000001c
	#define BIO_CMD_SEEK					0x0000001d
	#define BIO_CMD_FILE					0x0000001e
	#define BIO_CMD_PACKETS					0x0000001f
	#define BIO_CMD_FILE_NEXT				0x00000020
	#define BIO_CMD_PACKETS_NEXT			0x00000021
	#define BIO_CMD_DATA_HEADER				0x00000022
	#define BIO_CMD_BAD_CRAFT				0x00000023

	#define BIO_CMD_EX						0x00001000


	#define BIO_CMD(prefix, cmd)			BIO_CMD_##prefix_##cmd
	#define BIO_VALUE(v)					BIO_CMD_EX + (v)


	typedef enum tagB_IO_Operation : unsigned int
	{
		BIO_None							= 0x00000000,

		BIO_Accept							= 0x00000001,
		BIO_Close							= 0x00000002,
		BIO_Connect							= 0x00000004,
		BIO_Disconnect						= 0x00000008,
		BIO_Read							= 0x00000010,
		BIO_Write							= 0x00000020,
		BIO_Retire                          = 0x00000040,
		BIO_Continue                        = 0x00000080,		
		BIO_Abort							= 0x00000100,
		BIO_File							= 0x00000200,
		BIO_Packet							= 0x00000400,

		BIO_Error							= 0x10000000
	} B_IO_Operation, *LPB_IO_Operation;

	typedef enum tagB_IO_Violation : unsigned int
	{
		BIO_VIO_Zombie						= 0x00001000,
		BIO_VIO_Data						= 0x00001001
	} B_IO_Violation, *LPB_IO_Violation;

	typedef enum tagB_IO_Violation_Action : unsigned int
	{
		BIO_VIO_A_LetGo						= 0x00001000,
		BIO_VIO_A_ShutDown					= 0x00001001,

		BIO_VIO_A_BlackList					= 0x00001021,
		BIO_VIO_A_YellowList				= 0x00001022,
		BIO_VIO_A_WhiteList					= 0x00001023,

		BIO_VIO_A_RemoveFromList			= 0x00001041
	} B_IO_Violation_Action, *LPB_IO_Violation_Action; 

	typedef enum tagB_IO_Read_Mode
	{
		BIO_RM_Serial = 1,
		BIO_RM_Parallel
	} B_IO_Read_Mode, *LPB_IO_Read_Mode;

	typedef enum tagB_IO_Bad_Craft_Type : unsigned int
	{
		BIO_BDCR_DataChannel_LessData = 0x00000001,
		BIO_BDCR_DataChannel_Failed,
		BIO_BDCR_DataChannel_InvalidSecurity,
		BIO_BDCR_DataChannel_DecryptFailed,
		BIO_BDCR_DataChannel_HashFailed,
		BIO_BDCR_DataChannel_SigFailed,

		BIO_BDCR_Handler_Failed = 0x00010001
	} B_IO_Bad_Craft_Type, *LPB_IO_Bad_Craft_Type;

	typedef enum tagB_IO_Rule_PreConfig : unsigned char
	{
		BIO_RULE_PC_Closed = 0x00,
		BIO_RULE_PC_Default = 0x01,
		BIO_RULE_PC_Collection = 0x02,
		BIO_RULE_PC_User = 0x20
	} B_IO_Rule_PreConfig, *LPB_IO_Rule_PreConfig;


	typedef struct tagB_DATA_HEADER_CRAFT
	{
		void*				pRaw;
		void*				pHeader;
	} B_DATA_HEADER_CRAFT, *LPB_DATA_HEADER_CRAFT;
	
	
}


#endif // _BIODEF_H_
