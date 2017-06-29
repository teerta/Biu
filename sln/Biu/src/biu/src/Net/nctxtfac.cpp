#include "stdafx.h"
#include "bnctxt.h"
#include "bbfunc.h"
#include <cassert>


using namespace std;
using namespace Biu;


// BNHostFactory

BNHostFactory::BNHostFactory()
{
}

BNHostFactory::~BNHostFactory()
{
}

//BNPeerContext* BNHostFactory::CreatePeerContext()
//{
//	return new BNPeerContext();
//}

BNServerContext* BNHostFactory::CreateServerContext()
{
	return new BNServerContext();
}

//BNHostContext* BNHostFactory::CreateHostContext()
//{
//	return new BNHostContext();
//}

BNClientContext* BNHostFactory::CreateClientContext()
{
	return new BNClientContext();
}

//BNUserContext* BNHostFactory::CreateUserContext()
//{
//	return new BNUserContext();
//}
//
//BNUserStatistics* BNHostFactory::CreateUserStatistics()
//{
//	return new BNUserStatistics();
//}


// BNServiceFactory

BNServiceFactory::BNServiceFactory()
{
}

BNServiceFactory::~BNServiceFactory()
{
}

BNPeerContext* BNServiceFactory::CreatePeerContext()
{
	return new BNPeerContext();
}


BNUserContext* BNServiceFactory::CreateUserContext()
{
	return new BNUserContext();
}

BNUserStatistics* BNServiceFactory::CreateUserStatistics()
{
	return new BNUserStatistics();
}
