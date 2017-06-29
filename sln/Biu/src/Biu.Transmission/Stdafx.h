// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS


// Windows

#include <windows.h>
#include <tchar.h>
//#include <WinSock2.h>
//#include <WS2tcpip.h>
#include <WinDNS.h>


#pragma comment (lib, "kernel32")
#pragma comment (lib, "user32")
#pragma comment (lib, "gdi32")
#pragma comment (lib, "winspool")
#pragma comment (lib, "comdlg32")
#pragma comment (lib, "ws2_32")


// CLR

#include <vcclr.h>
#include <msclr\lock.h>

using namespace msclr;


// STL

#include <list>
#include <map>
#include <algorithm>
#include <regex>
#include <tuple>

using namespace std;


// ATL

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
#include <atlfile.h>
#include <atlcoll.h>
#include <atlcomtime.h>

using namespace ATL;


// Biu

#include "bmarco.h"
#include "bashley.h"
#include "bmshack.h"
#include "bnio.h"

#ifdef _DEBUG
#pragma comment(lib, "biud")
#else
#pragma comment(lib, "biu")
#endif

using namespace Biu;


// Linker

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
