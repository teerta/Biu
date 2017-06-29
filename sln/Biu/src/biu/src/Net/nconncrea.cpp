
#include "stdafx.h"
#include "bnpar.h"
#include "bptl.h"
#include "biohand.h"


using namespace Biu;


BNConnectionCreation::BNConnectionCreation()
	: trait(BSTR_Normal)
	, sockType(BST_IPv4_TCP)
	, pProtocol(NULL)
	, pHandler(NULL)
	, nAccepts(10)
	, llTimeoutSeconds(-1)
	, nBacklog(64)
	, llVar(0)
	, nRef(0)
	, bCompatibleInitializationState(false)
	, pSec(NULL)
{
	sa = { 0 };
	saRemote = { 0 };

	sa.sin_family = AF_INET;
	sa.sin_addr.S_un.S_addr = INADDR_ANY;

	saRemote.sin_family = AF_INET;
}

BNConnectionCreation::~BNConnectionCreation()
{

}

