
#ifndef _BPEERREPRE_H_
#define _BPEERREPRE_H_


#include "biu.h"
#include "bbase.h"


namespace Biu {

	
	// Class BPeerRepresentation

	class BIU_API BPeerRepresentation
	{
	public:
		BPeerRepresentation();
		BPeerRepresentation(sockaddr* psa);
		BPeerRepresentation(const BPeerRepresentation& xpr);
		virtual ~BPeerRepresentation();

		BPeerRepresentation& operator =(const BPeerRepresentation& xpr);
		bool operator ==(const BPeerRepresentation& xpr);
		bool operator !=(const BPeerRepresentation& xpr);
		operator long long() const;

		void SetSockAddr(sockaddr* psa);
		void Clear();

		size_t GetAddress(char* pszAddr, size_t cbAddr);

		// Network byte order
		inline long long GetAddressLong() const { return m_llAddr; }

		inline unsigned short GetPort() const { return m_usPort; }
		inline const unsigned int GetID() const
		{
			return (unsigned int)(m_llAddr * 100 + m_usPort);
		}

	private:
		long long m_llAddr;
		unsigned short m_usPort;
		char m_szAddr[B_MAX_NAME_SIZE];

		void Copy(BPeerRepresentation& xpr);
	};


}


#endif // _BPEERREPRE_H_
