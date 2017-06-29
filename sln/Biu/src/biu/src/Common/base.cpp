
#include "stdafx.h"
#include "bbase.h"

#if defined(_WINDOWS)

#include "biu.h"

#elif defined(__linux__) || defined(__APPLE__)

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif


using namespace Biu;


const size_t		_B_Size_Of_SockAddr_Storage						= sizeof(sockaddr_storage);
const size_t		_B_Size_Of_AcceptEx_Addr						= _B_Size_Of_SockAddr_Storage + 16;


const size_t		Biu::B_Size_Of_BYTE								= sizeof(char);
const size_t 		Biu::B_Size_Of_WORD = sizeof(short);
const size_t		Biu::B_Size_Of_DWORD							= sizeof(int);
const size_t		Biu::B_Size_Of_QWORD							= sizeof(long long);
const size_t		Biu::B_Size_Of_SockAddr_In						= sizeof(sockaddr_in);
const size_t		Biu::B_Size_Of_SockAddr_Storage					= _B_Size_Of_SockAddr_Storage;

#ifdef _WINDOWS

const size_t		Biu::B_Size_Of_Socket							= sizeof(SOCKET);
const size_t		Biu::B_Size_Of_FileTime							= sizeof(FILETIME);
const size_t		Biu::B_Size_Of_Overlapped						= sizeof(OVERLAPPED);
const size_t		Biu::B_Size_Of_AcceptEx_Addr					= _B_Size_Of_AcceptEx_Addr;
const size_t		Biu::B_Size_Of_AcceptEx_Available_Buffer		= B_MAX_BUFF_SIZE - (2 * _B_Size_Of_AcceptEx_Addr);

#endif // _WINDOWS
