#include "stdafx.h"
#include "IOSecurityContext.h"


using namespace Biu::Transmission;


IOSecurityContext::IOSecurityContext()
	: m_pInner(NULL)
{
}

IOSecurityContext::~IOSecurityContext()
{
	this->!IOSecurityContext();
}

IOSecurityContext::!IOSecurityContext()
{
	m_pInner = NULL;
}

bool IOSecurityContext::ChangeRemoateKey(cli::array<byte>^ rawKey)
{
	LPBYTE lp = new BYTE[rawKey->Length];
	for (int i = 0; i < rawKey->Length; i++)
	{
		lp[i] = rawKey[i];
	}

	bool bRet = m_pInner->ChangeRemoteKeyStatic(lp, rawKey->Length);
	SAFE_DELETE_ARRAY(lp);

	return bRet;
}

cli::array<byte>^ IOSecurityContext::ExportKey()
{
	LPBYTE lp = NULL;
	DWORD cb = m_pInner->ExportKeyStatic(&lp);
	if (!cb)
	{
		return nullptr;
	}

	cli::array<byte>^ rawKey = gcnew cli::array<byte>(cb);
	for (int i = 0; i < rawKey->Length; i++)
	{
		rawKey[i] = lp[i];
	}
	SAFE_DELETE_ARRAY(lp);

	return rawKey;
}
