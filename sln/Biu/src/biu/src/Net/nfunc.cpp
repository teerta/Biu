
#include "stdafx.h"
#include "bnfunc.h"


using namespace Biu;


bool 
Biu::
StartUpWSA()
{
	WSADATA wsd;
	return !WSAStartup(MAKEWORD(2, 2), &wsd);
}

bool 
Biu::
CleanUpWSA()
{
	return !WSACleanup();
}
