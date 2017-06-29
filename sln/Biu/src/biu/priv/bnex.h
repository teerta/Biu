
#ifndef _BNEX_H_
#define _BNEX_H_


#ifdef _WINDOWS


#include "biu.h"


namespace Biu {


	// Forwarding

	struct I_Peer;


	// Class BSockExMethod

	class BSockExMethod
	{
	public:
		BSockExMethod();
		BSockExMethod(BSockExMethod& xsem);
		virtual~BSockExMethod();

		BSockExMethod& operator =(BSockExMethod& xsem);

		bool Load(I_Peer* pPeerIntern);

		inline LPFN_ACCEPTEX AcceptEx() { return m_fnAcceptEx; }
		inline LPFN_CONNECTEX ConnextEx() { return m_fnConnectEx; }
		inline LPFN_DISCONNECTEX DisconnectEx() { return m_fnDisconnectEx; }
		inline LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockAddrs() { return m_fnGetAccepeExSockAddrs; }
		//inline LPFN_TRANSMITFILE TransmitFile() { return m_fnTransmitFile; }

	private:
		LPFN_ACCEPTEX				m_fnAcceptEx;
		LPFN_CONNECTEX				m_fnConnectEx;
		LPFN_DISCONNECTEX			m_fnDisconnectEx;
		LPFN_GETACCEPTEXSOCKADDRS	m_fnGetAccepeExSockAddrs;
		//LPFN_TRANSMITFILE			m_fnTransmitFile;

		void Copy(BSockExMethod& xsem);
	};


}

#endif// _WINDOWS


#endif // _BNEX_H_
