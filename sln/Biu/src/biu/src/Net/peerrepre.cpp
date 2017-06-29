
#include "stdafx.h"
#include "bpeerrepre.h"


using namespace Biu;


// BPeerRepresentation

BPeerRepresentation::BPeerRepresentation()
	: m_llAddr(0)
	, m_usPort(0)
{
	memset(m_szAddr, 0, B_MAX_NAME_SIZE);
}

BPeerRepresentation::BPeerRepresentation(sockaddr* psa)
{
	SetSockAddr(psa);
}

BPeerRepresentation::BPeerRepresentation(const BPeerRepresentation& xpr)
{
	Copy(*const_cast<BPeerRepresentation*>(&xpr));
}

BPeerRepresentation::~BPeerRepresentation()
{
}

BPeerRepresentation& BPeerRepresentation::operator =(const BPeerRepresentation& xpr)
{
	Copy(*const_cast<BPeerRepresentation*>(&xpr));
	return *this;
}

bool BPeerRepresentation::operator ==(const BPeerRepresentation& xpr)
{
	return
			m_llAddr == xpr.GetAddressLong() &&
			m_usPort == xpr.GetPort();
}

bool BPeerRepresentation::operator !=(const BPeerRepresentation& xpr)
{
	return !operator ==(xpr);
}

BPeerRepresentation::operator long long() const
{
	return (unsigned int)(m_llAddr * 100 + m_usPort);
}

void BPeerRepresentation::SetSockAddr(sockaddr* psa)
{
	sockaddr_in* p = reinterpret_cast<sockaddr_in*>(psa);

	m_usPort = ntohs(p->sin_port);

	char* pAddr = inet_ntoa(p->sin_addr);
	if (!pAddr)
	{
		return;
	}

#ifdef _WINDOWS
	strcpy_s(m_szAddr, pAddr);
#else
	strcpy(m_szAddr, pAddr);
#endif

	//m_ulAddr = ntohl(p->sin_addr.S_un.S_addr);
	m_llAddr = p->sin_addr.s_addr;
}

void BPeerRepresentation::Clear()
{
	m_llAddr = 0;
	m_usPort = 0;
	memset(m_szAddr, 0, B_MAX_NAME_SIZE);
}

void BPeerRepresentation::Copy(BPeerRepresentation& xpr)
{
	if (this != &xpr)
	{
		m_llAddr = xpr.GetAddressLong();
		m_usPort = xpr.GetPort();
#ifdef _WINDOWS
		strcpy_s(m_szAddr, xpr.m_szAddr);
#else
		strcpy(m_szAddr, xpr.m_szAddr);
#endif
	}
}

size_t BPeerRepresentation::GetAddress(char* pszAddr, size_t cbAddr)
{
	size_t cb = strlen(m_szAddr);
	if (!pszAddr)
	{
		return cb;
	}

#ifdef _WINDOWS
	strcpy_s(pszAddr, cbAddr, m_szAddr);
#else
	strcpy(pszAddr, m_szAddr);
#endif

	return cb;
}
