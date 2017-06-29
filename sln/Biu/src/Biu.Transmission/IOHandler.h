
#pragma once


#include "Interface.h"
#include "Delegate.h"
#include "bmshack.h"
#include "bah.h"
#include "PeerData.h"
#include "EventObject.h"


using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Biu;


namespace Biu { namespace Transmission {


	// Forwarding

	class NHandlerChannel;	

	ref class MemoryShack;
	ref class DataInfo;
	ref class WaitDataInfo;


	// Ref Class IOCommand

	public ref class IOCommand
	{
	public:
		IOCommand();


		//event Func<Peer^, bool>^ NewHandle;
		event Action<Peer^, unsigned long long, long>^ Error
		{
			void add(Action<Peer^, unsigned long long, long>^ handler)
			{
				_error += handler;
			}

			void remove(Action<Peer^, unsigned long long, long>^ handler)
			{
				_error -= handler;
			}

			void raise(Peer^ peer, unsigned long long ecs, long error)
			{
				_error(peer, ecs, error);
			}
		}

		event Action<Peer^>^ Timeout
		{
			void add(Action<Peer^>^ handler)
			{
				_timeout += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_timeout -= handler;
			}

			void raise(Peer^ peer)
			{
				_timeout(peer);
			}
		}

		event Action<Peer^, long>^ WriteDone
		{
			void add(Action<Peer^, long>^ handler)
			{
				_writeDone += handler;
			}

			void remove(Action<Peer^, long>^ handler)
			{
				_writeDone -= handler;
			}

			void raise(Peer^ peer, long error)
			{
				_writeDone(peer, error);
			}
		}

		event Action<Peer^>^ CloseProactive
		{
			void add(Action<Peer^>^ handler)
			{
				_closeProactive += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_closeProactive -= handler;
			}

			void raise(Peer^ peer)
			{
				_closeProactive(peer);
			}
		}

		event Action<Peer^>^ ClosePassive
		{
			void add(Action<Peer^>^ handler)
			{
				_closePassive += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_closePassive -= handler;
			}

			void raise(Peer^ peer)
			{
				_closePassive(peer);
			}
		}

		event Action<Peer^, DataInfo^>^ File
		{
			void add(Action<Peer^, DataInfo^>^ handler)
			{
				_file += handler;
			}

			void remove(Action<Peer^, DataInfo^>^ handler)
			{
				_file -= handler;
			}

			void raise(Peer^ peer, DataInfo^ dataInfo)
			{
				_file(peer, dataInfo);
			}
		}

		event Action<Peer^, DataContinuation^, WaitDataInfo^>^ FileNext
		{
			void add(Action<Peer^, DataContinuation^, WaitDataInfo^>^ handler)
			{
				_fileNext += handler;
			}

			void remove(Action<Peer^, DataContinuation^, WaitDataInfo^>^ handler)
			{
				_fileNext -= handler;
			}

			void raise(Peer^ peer, DataContinuation^ continuation, WaitDataInfo^ waitDataInfo)
			{
				_fileNext(peer, continuation, waitDataInfo);
			}
		}

		//event Action<Peer^, DataInfo^>^ DataHeader
		//{
		//	void add(Action<Peer^, DataInfo^>^ handler)
		//	{
		//		_dataHeader += handler;
		//	}

		//	void remove(Action<Peer^, DataInfo^>^ handler)
		//	{
		//		_dataHeader -= handler;
		//	}

		//	void raise(Peer^ peer, DataInfo^ dataInfo)
		//	{
		//		_dataHeader(peer, dataInfo);
		//	}
		//}

		event Action<Peer^>^ Connected
		{
			void add(Action<Peer^>^ handler)
			{
				_connected += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_connected -= handler;
			}

			void raise(Peer^ peer)
			{
				_connected(peer);
			}
		}

		event Action<Peer^, BoolArg^>^ Accepted
		{
			void add(Action<Peer^, BoolArg^>^ handler)
			{
				_accepted += handler;
			}

			void remove(Action<Peer^, BoolArg^>^ handler)
			{
				_accepted -= handler;
			}

			void raise(Peer^ peer, BoolArg^ arg)
			{
				return _accepted(peer, arg);
			}
		}

		event Action<Peer^>^ ConnectRefused
		{
			void add(Action<Peer^>^ handler)
			{
				_connectRefused += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_connectRefused -= handler;
			}

			void raise(Peer^ peer)
			{
				_connectRefused(peer);
			}
		}

		event Action<Peer^>^ NetNameUnavailable
		{
			void add(Action<Peer^>^ handler)
			{
				_netNameUnavailable += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_netNameUnavailable -= handler;
			}

			void raise(Peer^ peer)
			{
				_netNameUnavailable(peer);
			}
		}

		event Action<Peer^>^ HostUnavailable
		{
			void add(Action<Peer^>^ handler)
			{
				_hostUnavailable += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_hostUnavailable -= handler;
			}

			void raise(Peer^ peer)
			{
				_hostUnavailable(peer);
			}
		}

		event Action<Peer^>^ NetUnavailable
		{
			void add(Action<Peer^>^ handler)
			{
				_netUnavailable += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_netUnavailable -= handler;
			}

			void raise(Peer^ peer)
			{
				_netUnavailable(peer);
			}
		}

		event Action<Peer^>^ ConnectionReset
		{
			void add(Action<Peer^>^ handler)
			{
				_connectionReset += handler;
			}

			void remove(Action<Peer^>^ handler)
			{
				_connectionReset -= handler;
			}

			void raise(Peer^ peer)
			{
				_connectionReset(peer);
			}
		}


	internal:
		void OnError(Peer^ peer, unsigned long long ecs, long error);
		void OnTimeout(Peer^ peer);
		void OnWriteDone(Peer^ peer, long size);
		void OnCloseProactive(Peer^ peer);
		void OnClosePassive(Peer^ peer);
		void OnFile(Peer^ peer, DataInfo^ dataInfo);
		void OnFileNext(Peer^ peer, DataContinuation^ continuation, WaitDataInfo^ waitDataInfo);
		//void OnDataHeader(Peer^ peer, DataInfo^ dataInfo);

		void OnConnected(Peer^ peer);
		void OnAccepted(Peer^ peer, BoolArg^ arg);
		void OnConnectedRefused(Peer^ peer);
		void OnNetNameUnavailable(Peer^ peer);
		void OnHostUnavailable(Peer^ peer);
		void OnNetAvailable(Peer^ peer);
		void OnConnectionReset(Peer^ peer);


	private:
		Action<Peer^, unsigned long long, long>^							_error;
		Action<Peer^>^									_timeout;
		Action<Peer^, long>^							_writeDone;
		Action<Peer^>^									_closeProactive;
		Action<Peer^>^									_closePassive;
		Action<Peer^, DataInfo^>^						_file;
		Action<Peer^, DataContinuation^, WaitDataInfo^>^			_fileNext;
		//Action<Peer^, DataInfo^>^						_dataHeader;

		Action<Peer^>^									_connected;
		Action<Peer^, BoolArg^>^									_accepted;
		Action<Peer^>^									_connectRefused;
		Action<Peer^>^									_netNameUnavailable;
		Action<Peer^>^									_hostUnavailable;
		Action<Peer^>^									_netUnavailable;
		Action<Peer^>^									_connectionReset;
	};


	// Ref Class IOHandler

	public ref class IOHandler
	{
	public:
		__clrcall IOHandler(unsigned long long identification, unsigned int maxPacketSize, unsigned int minPacketSize);
		__clrcall ~IOHandler();
		__clrcall !IOHandler();


		event BiuIOHandler^ IO;


		property IOCommand^ Command
		{
			IOCommand^ __clrcall get() { return m_cmd; }
		}

		//property BiuHandlerChannel^ Channel;
		property MemoryShack^ Shack;


	internal:
		NHandlerChannel* m_pChannel;
		Ashley_ChannelHandler* m_pInner;


		bool OnIO(unsigned long long action, Peer^ peer, IOOperation% operation);


	private:
		IOCommand^ m_cmd;
		GCHandle m_hThis;
	};



}}
