
#ifndef _BNDEF_H_
#define _BNDEF_H_


#include "biodef.h"


namespace Biu {

	
	#define BIO_CMD_NET_SEND								BIO_VALUE(0x0001)
	#define BIO_CMD_NET_RECV								BIO_VALUE(0x0002)
	#define BIO_CMD_NET_ACCEPT								BIO_VALUE(0x0003)
	#define BIO_CMD_NET_CONNECT								BIO_VALUE(0x0004)
	#define BIO_CMD_NET_DISCONNECT							BIO_VALUE(0x0005)

	#define BIO_CMD_NET_OUTTERCREATE						BIO_VALUE(0x0006)

	#define BIO_CMD_NET_CONNECTED							BIO_VALUE(0x0010)
	#define BIO_CMD_NET_ACCEPTED							BIO_VALUE(0x0011)
	#define BIO_CMD_NET_CONNECT_REFUSED						BIO_VALUE(0x0012)
	#define BIO_CMD_NET_NETNAME_UNAVAILABLE					BIO_VALUE(0x0013)
	#define BIO_CMD_NET_HOST_UNAVAILABLE					BIO_VALUE(0x0014)
	#define BIO_CMD_NET_CONNECTION_RESET					BIO_VALUE(0x0015)
	#define BIO_CMD_NET_UNAVAILABLE							BIO_VALUE(0x001b)
	#define BIO_CMD_NET_MALICIOUS_CONNECTION				BIO_VALUE(0x001c)


	typedef enum tagB_Sock_Type
	{
		BST_IPv4_TCP = 1,
		BST_IPv4_UDP,
		BST_IPv6_TCP = 20,
		BST_IPv6_UDP,
		BST_Unix_Domain_TCP = 50,
		BST_Unix_Domain_UDP,
		BST_Raw = 1000
	} B_Sock_Type, *LPB_Sock_Type;

	typedef enum tagB_Sock_Connect_Type
	{
		BSST_Connection_Oriented = 1,
		BSST_Connectionless,
		BSST_Raw = 100
	} B_Sock_Connect_Type, *LPB_Sock_Connect_Type;

	typedef enum tagB_Peer_Close_Flag : unsigned int
	{
		BPCF_None = 0,

		BPCF_Close = 1,
		BPCF_Retire,


		BPCF_Abort					= 0x00010000,
		BPCF_Reuse					= 0x00020000,
		BPCF_Terminate				= 0x00040000
	} B_Peer_Close_Flag, *LPB_Peer_Close_Flag;

	typedef enum tagB_Peer_Transmit_Result
	{
		BPTR_True = 0,
		BPTR_False,
		BPTR_Closing,
		BPTR_Wait,
		BPTR_Finish,
		BPTR_Pending
	} B_Peer_Transmit_Result, *LPB_Peer_Transmit_Result;

	typedef enum tagB_Sock_Trait
	{
		BSTR_Normal					= 1,
		BSTR_Data
	} B_Sock_Trait, *LPB_Sock_Trait;

	typedef enum tagB_Malicious_Connection_Type
	{
		BMCT_HalfOpen				= 1,
		BMCT_BlackList,
		BMCT_YellowList,
		BMCT_Focused
	} B_Malicious_Connection_Type, *LPMalicious_Connection_Type;



	struct BMemoryBrick;


	typedef struct tagB_Sock_Outter_Create
	{
		BMemoryBrick*		pBrick;
		void*				pObj;
		void*				pHeader;
	} B_Sock_Outter_Create, *LPB_Sock_Outter_Create;


}


#endif // _BNDEF_H_
