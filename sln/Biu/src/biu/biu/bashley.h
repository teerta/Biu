
#ifndef _BASHLEY_H_
#define _BASHLEY_H_


#include "biu.h"
#include "bptl.h"
#include <cassert>

#ifdef _WINDOWS
#include <winbase.h>
#endif


using namespace std;


namespace Biu {

	

	#define B_ASHLEY_V_1_0                                              0x00010000
	#define B_ASHLEY_V_1_1                                              0x00010100

	#define B_ASHLEY_NONE                                               0x00

	#define B_ASHLEY_FL_RECOG	                                        0x00000001
	#define B_ASHLEY_FL_ACK                                             0x00000002
	#define B_ASHLEY_FL_VERIFICATION                                    0x00000004
	#define B_ASHLEY_FL_SECURITY                                        0x00000008
	#define B_ASHLEY_FL_PART                                            0x00000010
	#define B_ASHLEY_FL_DISPLACEMENT                                    0x00000020
	//#define B_ASHLEY_FL_FILE		                                    0x00000040
	//#define B_ASHLEY_FL_FLAGEX                                        0x00000080

	#define B_ASHLEY_VERI_MD5                                           0x0001

	#define B_ASHLEY_ACK_OK                                             0x00000001
	#define B_ASHLEY_ACK_FAILED                                         0x00000002
	#define B_ASHLEY_ACK_VERIFICATION_FAILED                            0x00000100
	#define B_ASHLEY_ACK_DISPLACEMENT_FAILED                            0x00000101
	#define B_ASHLEY_ACK_SECURITY_FAILED                                0x00000102
	#define B_ASHLEY_ACK_PART_ID_INVALID                                0x00000110
	#define B_ASHLEY_ACK_PART_OUT_OF_RANGE                              0x00000111
	#define B_ASHLEY_ACK_PART_RECEIVED                                  0x00000112
	#define B_ASHLEY_ACK_PART_RETRANSPORT                               0x00000113
	#define B_ASHLEY_ACK_PART_DISCARD                                   0x00000114

	#define B_ASHLEY_SEC_P4_1											0x0001
	#define B_ASHLEY_SEC_P4S											0x0002


	#pragma pack(push)
	#pragma pack(1)

	typedef struct tagB_ASHLEY_HEADER_BASE
	{
		unsigned int													Magic;
		unsigned int													Version;
		unsigned int													Size;
		unsigned int													Flag;
	} B_ASHLEY_HEADER_BASE, *LPB_ASHLEY_HEADER_BASE;

	typedef struct tagB_ASHLEY_HEADER_RECOG
	{
		unsigned long long                                              Identification;
		unsigned long long                                              Processing;		
		unsigned short                                                  ExHeaderSize;
	} B_ASHLEY_HEADER_RECOG, *LPB_ASHLEY_HEADER_RECOG;

	typedef struct tagB_ASHLEY_HEADER_FLAGEX
	{
		unsigned short                                                  FleType;
	} B_ASHLEY_HEADER_FLAGEX, *LPB_ASHLEY_HEADER_FLAGEX;

	typedef struct tagB_ASHLEY_HEADER_ACK
	{
		unsigned int													AckResult;
	} B_ASHLEY_HEADER_ACK, *LPB_ASHLEY_HEADER_ACK;

	typedef struct tagB_ASHLEY_HEADER_VERIFICATION
	{
		unsigned short                                                  VeriType;
		union
		{
			unsigned char                                               VeriHash16[16];
		};
	} B_ASHLEY_HEADER_VERIFICATION, *LPB_ASHLEY_HEADER_VERIFICATION;

	typedef struct tagB_ASHLEY_HEADER_SECURITY
	{
		unsigned short                                                  Type;
		union
		{
			struct
			{
				short													HashSize;
				unsigned char*											Hash;
				short													SigSize;
				unsigned char*											Sig;
			} P4;

			struct
			{
				short													HashSize;
				unsigned char*											Hash;
				short													SigSize;
				unsigned char*											Sig;
				short													KeySize;
				unsigned char*											Key;
			} P4S;
		};
	} B_ASHLEY_HEADER_SECURITY, *LPB_ASHLEY_HEADER_SECURITY;

	typedef struct tagB_ASHLEY_HEADER_PART
	{
		long long                                                       ID;
		unsigned long long                                              Total;
		unsigned long long                                              Current;
	} B_ASHLEY_HEADER_PART, *LPB_ASHLEY_HEADER_PART;

	typedef struct tagB_ASHLEY_HEADER_DISPLACEMENT
	{
		unsigned short                                                  DisType;
	} B_ASHLEY_HEADER_DISPLACEMENT, *LPB_ASHLEY_HEADER_DISPLACEMENT;

	//typedef struct tagB_ASHLEY_HEADER_FILE
	//{
	//	unsigned long long												Size;
	//	int																ID;
	//} B_ASHLEY_HEADER_FILE, *LPB_ASHLEY_HEADER_FILE;

	#pragma pack(pop)

	typedef struct tagB_ASHLEY_HEADER
	{
		B_ASHLEY_HEADER_BASE                                            Base;
		B_ASHLEY_HEADER_SECURITY                                        Sec;
		B_ASHLEY_HEADER_RECOG											Recog;		
		B_ASHLEY_HEADER_ACK                                             Ack;
		B_ASHLEY_HEADER_VERIFICATION                                    Veri;		
		B_ASHLEY_HEADER_PART                                            Pa;
		B_ASHLEY_HEADER_DISPLACEMENT                                    Dis;
		B_ASHLEY_HEADER_FLAGEX                                          Fle;
		//B_ASHLEY_HEADER_FILE											File;
	} B_ASHLEY_HEADER, *LPB_ASHLEY_HEADER;


	static const unsigned int B_Ashley_Magic							= 0x41534850;	// ASHP

	static const size_t B_Ashley_Size_Of_AshleyHeader					= sizeof(B_ASHLEY_HEADER);
	static const size_t B_Ashley_Size_Of_BaseHeader						= sizeof(B_ASHLEY_HEADER_BASE);
	static const size_t B_Ashley_Size_Of_Recog							= sizeof(B_ASHLEY_HEADER_RECOG);
	static const size_t B_Ashley_Size_Of_FlagEx							= sizeof(B_ASHLEY_HEADER_FLAGEX);
	static const size_t B_Ashley_Size_Of_Ack                            = sizeof(B_ASHLEY_HEADER_ACK);
	static const size_t B_Ashley_Size_Of_Verification					= sizeof(B_ASHLEY_HEADER_VERIFICATION);
	static const size_t B_Ashley_Size_Of_Part							= sizeof(B_ASHLEY_HEADER_PART);
	static const size_t B_Ashley_Size_Of_Displacement					= sizeof(B_ASHLEY_HEADER_DISPLACEMENT);
	//static const size_t B_Ashley_Size_Of_File							= sizeof(B_ASHLEY_HEADER_FILE);
	
	static const size_t B_Ashley_Size_Of_Security_Base					= sizeof(short);
	static const size_t B_Ashley_Size_Of_Security_P4					= sizeof(short) * 2;
	static const size_t B_Ashley_Size_Of_Security_P4S					= sizeof(short) * 3;

	static const size_t B_Ashley_Size_Of_Type8							= sizeof(char);
	static const size_t B_Ashley_Size_Of_Type16							= sizeof(short);
	static const size_t B_Ashley_Size_Of_Type32							= sizeof(int);
	static const size_t B_Ashley_Size_Of_Type64							= sizeof(long long);

	static const size_t B_Ashley_Size_Of_VeriHash16						= 16;

	static const size_t B_Ashley_Pos_Base_Size							= sizeof(int) * 2;


	// Class BAshleyP

	class BIU_API BAshley : public IBDataChannel
	{
	public:
		BAshley();
		virtual ~BAshley();
		
		virtual B_Data_Channel_Result ParseData(BMemoryBrick* pBrick, size_t& nDataPos, void* pSec = NULL, BProtocolRestriction* pRestriction = NULL);
		
		virtual bool CreateObj(BMemoryBrick* pBrick, IBSerializable* pObj, void* pVal = NULL, void* pSec = NULL);
		virtual bool GetObj(IBSerializable* pObj);
		
		//////////////////////// Not Implemented //////////////////////////////
		virtual void CreatePod(BMemoryBrick* pBrick, const void* pData, size_t cbData);
		virtual bool GetPod(void* pData, size_t& cbRead);
		virtual size_t GetPodSize();
		/////////////////////////////////////////////////////////////////////
		
		virtual IBDataChannel* Clone();        
		virtual void ChangeMemory(BMemoryBrick* pBrick);                
		virtual void SetMemoryModel(BMemoryShack_char* pShack, BSerializer::Read_Function func);

		virtual BSerializer::Read_Function GetReadFunc();

		
		//inline void ChangeSendingBrick(BMemoryBrick* pBrick) { m_bmsSend.ChangeBrick(pBrick); }
		//inline void ChangeReceivingBrick(BMemoryBrick* pBrick) { m_bmsRecv.ChangeBrick(pBrick); }

		inline LPB_ASHLEY_HEADER GetRecvHeader() { return &m_hdrRecv; }
		inline unsigned long long GetProcessing() { return m_hdrRecv.Recog.Processing; }

		inline BSerializer* GetRecvSerializer() { return &m_bmsRecv; }
		inline BSerializer* GetSendSerializer() { return &m_bmsSend; }

	protected:
		BSerializer m_bmsSend;
		BSerializer m_bmsRecv;

		B_ASHLEY_HEADER m_hdrRecv;
	};


	// Functions

	static inline void BAshley_Initialize(
		LPB_ASHLEY_HEADER pAshleyHeader,        
		unsigned long long ullProcessing = B_ASHLEY_NONE,
		unsigned long long ullIdenfication = B_ASHLEY_NONE,
		unsigned int ulVersuin = B_ASHLEY_V_1_1, 
		unsigned int flag = B_ASHLEY_NONE
	)
	{
#ifdef _WINDOWS
		ZeroMemory(pAshleyHeader, B_Ashley_Size_Of_AshleyHeader);
#else
		bzero(pAshleyHeader, B_Ashley_Size_Of_AshleyHeader);
#endif		
		pAshleyHeader->Base.Flag = flag | B_ASHLEY_FL_RECOG;
		pAshleyHeader->Base.Version = ulVersuin;
		pAshleyHeader->Recog.Processing = ullProcessing;
		pAshleyHeader->Recog.Identification = ullIdenfication;
	}
	
	static inline void BAshley_SetProcessing(LPB_ASHLEY_HEADER pHeader, unsigned long long ullProcessing)
	{
		pHeader->Recog.Processing = ullProcessing;
	}


}


#endif // _BASHLEY_H_
