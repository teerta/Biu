
#ifndef _BNPAR_H_
#define _BNPAR_H_


#include "biu.h"
#include "bndef.h"


namespace Biu {

	// Forwarding;

	struct IBDataChannel;
	class BIOHandler;
	class BIOSecurityContext;
	class BSecurityProvider;


	// Struct BNConnectionCreation

	struct BIU_API BNConnectionCreation
	{
		BNConnectionCreation();
		~BNConnectionCreation();


		B_Sock_Trait trait;
		B_Sock_Type sockType;
		IBDataChannel* pProtocol;
		BIOHandler* pHandler;
		sockaddr_in sa;
		sockaddr_in saRemote;
		unsigned int nAccepts;
		long long llTimeoutSeconds;
		int nBacklog;
		long long llVar;
		unsigned int nRef;
		bool bCompatibleInitializationState;
		BSecurityProvider* pSec;
	};


}


#endif // _BNPAR_H_
