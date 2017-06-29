
#include "stdafx.h"
#include "TransmissionManager.h"
#include "bnfunc.h"


using namespace Biu::Transmission;


// TransmissionManager

bool TransmissionManager::StartUpWSA()
{
	return Biu::StartUpWSA();
}

bool TransmissionManager::CleanUpWSA()
{
	return Biu::CleanUpWSA();
}
