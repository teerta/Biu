#pragma once


#include "bnio.h"
#include "Enum.h"
#include "bnpar.h"
#include "SecurityProvider.h"


using namespace System::Net;
using namespace Biu;


namespace Biu{ namespace Transmission {


	// Forwarding

	ref class Ashley;
	ref class IOHandler;
	ref class Peer;
	ref class ServerPeer;
	ref class DataPeer;
	ref class ClientDataPeer;


	// Ref Class ConnectionCreation

	public ref class ConnectionCreation
	{
	public:
		ConnectionCreation();

		property SocketTrait Trait;
		property SocketType Type;
		property Ashley^ Protocol;
		property IOHandler^ Handler;
		property IPEndPoint^ EndPoint;		
		property IPEndPoint^ RemoteEndPoint;
		property unsigned int AcceptCount;
		property long long Timeout;
		property int Backlog;
		property long long Var;
		property bool CompatibleInitializationState;
		property unsigned int Ref;
		property SecurityProvider^ SecurityProvider;

	internal:
		void ToNative(BNConnectionCreation* pncc);
	};


	// Ref Class NetIOExecutionUnit

	public ref class NetIOExecutionUnit
	{
	public:
		__clrcall NetIOExecutionUnit();
		__clrcall ~NetIOExecutionUnit();
		__clrcall !NetIOExecutionUnit();

		bool Listen(ConnectionCreation^ cc, unsigned int% cookie);
		bool Connect(ConnectionCreation^ cc, unsigned int% cookie);
		bool ConnectData(SocketTrait trait, unsigned int cookie);

		Peer^ AcquirePeerForConnecting(ConnectionCreation^ cc, unsigned int% cookie);
		bool Connect(Peer^ peer, ConnectionCreation^ cc, unsigned int% cookie);

		void CloseServer(unsigned int cookie);
		void CloseServerItem(unsigned int cookie, long long key);
		void CloseClient(unsigned int cookie, bool now);

		bool ContainsServer(unsigned int cookie);
		bool ContainsClient(unsigned int cookie);

		ServerPeer^ GetServer(unsigned int cookie);
		Peer^ GetClient(unsigned int cookie);

		DataPeer^ GetData(unsigned int cookie);
		ClientDataPeer^ GetClientData(unsigned int cookie);


	internal:
		BNetIOExecutionUnit* m_pUnit;
		bool				 m_bDetached;


	private:
		
	};


} }
