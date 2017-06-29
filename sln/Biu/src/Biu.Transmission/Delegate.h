
#pragma once


#include "Enum.h"
#include "Interface.h"


using namespace System;


namespace Biu { namespace Transmission {


	// Forwarding

	ref class Peer;
	ref class Ashley;


	// Delegates

	public delegate bool BiuIOHandler(/*BiuHandlerChannel^ handlerChannel, */unsigned long long action, Peer^ peer, IOOperation% operation);

	//public delegate bool BiuCommandHandler(BiuHandlerChannel^ handlerChannel, unsigned long long cmd, Peer^ peer, Object^ p1, Object^ p2);


}}
