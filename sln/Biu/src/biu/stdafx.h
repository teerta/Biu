#pragma once

#ifndef _STDAFX_H_
#define _STDAFX_H_


#define _CRT_RAND_S
#include <stdlib.h>


#ifdef _WINDOWS


	#pragma once

	#include "targetver.h"

	#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
	// Windows 头文件: 
	#include <windows.h>

	
	#define _WINSOCK_DEPRECATED_NO_WARNINGS

	#include <WinDNS.h>
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <MSWSock.h>
	#include <wincrypt.h>
	#include <shellapi.h>
	#include <TlHelp32.h>
	#include <winioctl.h>
	#include <IPHlpApi.h>


	#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

	#include <atlbase.h>
	#include <atlstr.h>
	#include <atlcoll.h>
	#include <ATLComTime.h>


	#pragma comment(lib, "ws2_32")
	#pragma comment(lib, "advapi32")
	#pragma comment(lib, "ntdll")
	#pragma comment(lib, "shell32")
	#pragma comment(lib, "wbemuuid")
	#pragma comment (lib, "Iphlpapi")



	#include <cassert>
	#include <list>
	#include <map>
	#include <algorithm>


	using namespace std;

	#ifdef _WINDOWS
	using namespace ATL;
	#endif


	#include "biu/berr.h"


	#pragma warning (disable: 4800)
	#pragma warning (disable: 4251)


#else

	// Dummy

#endif


#endif // _STDAFX_H_
