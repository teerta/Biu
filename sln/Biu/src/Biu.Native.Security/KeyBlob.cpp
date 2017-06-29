
#include "stdafx.h"
#include "KeyBlob.h"
#include "CryptKey.h"


using namespace Biu::Native::Security;
using namespace System::Linq;


KeyBlob::KeyBlob()
	: m_cbKey(0)
	, m_lpKey(NULL)
{
}

KeyBlob::KeyBlob(DWORD cbKey)
	: m_cbKey(cbKey)
	, m_lpKey(NULL)
{
	m_lpKey = new BYTE[cbKey];
}


KeyBlob::~KeyBlob()
{	
	this->!KeyBlob();
}


KeyBlob::!KeyBlob()
{
	SAFE_DELETE_ARRAY(m_lpKey);
	m_cbKey = 0;	
}


bool KeyBlob::Save(String^ path)
{
	bool bRet = false;
	Stream^ stream = nullptr;
	try
	{
		stream = gcnew FileStream(path, FileMode::Create, FileAccess::ReadWrite, FileShare::Write);

		bRet = Save(stream);
	}
	catch (Exception^)
	{
		throw;
	}
	finally
	{
		if (nullptr != stream)
		{
			stream->Close();
			stream = nullptr;
		}
	}

	return bRet;
}


bool KeyBlob::Save(Stream^ stream)
{
	if (NULL == m_lpKey || 0 == m_cbKey)
	{
		throw gcnew ArgumentException(_T("Blob is empty"));
	}

	bool bRet = false;
	try
	{
		for (DWORD i = 0; i < m_cbKey; i++)
		{
			stream->WriteByte(m_lpKey[i]);
		}

		bRet = true;
	}
	catch (...)
	{
		throw;
	}

	return bRet;
}


cli::array<Byte>^ KeyBlob::ToArray()
{
	if (!m_lpKey)
	{
		throw gcnew ArgumentException("Blob is empty");
	}

	auto buff = gcnew cli::array<Byte>(m_cbKey);
	for (DWORD i = 0; i < m_cbKey; i++)
	{
		buff[i] = m_lpKey[i];
	}

	return buff;
}


KeyBlob^ KeyBlob::FromFile(String^ path)
{
	KeyBlob^ blob = nullptr;
	Stream^ stream = nullptr;
	try
	{
		stream = gcnew FileStream(path, FileMode::Open, FileAccess::Read, FileShare::Write);
		auto buff = gcnew cli::array<Byte>(stream->Length);
		stream->Read(buff, 0, stream->Length);

		blob = FromBuffer(buff);
	}
	catch (Exception^)
	{
		throw;
	}
	finally
	{
		if (nullptr != stream)
		{
			stream->Close();
			stream = nullptr;
		}
	}

	return blob;
}


KeyBlob^ KeyBlob::FromBuffer(IEnumerable<Byte>^ coll)
{
	if (nullptr == coll)
	{
		throw gcnew ArgumentNullException(_T("coll is null"));
	}

	DWORD cb = Enumerable::Count(coll);
	auto blob = gcnew KeyBlob(cb);
	for (DWORD i = 0; i < cb; i++)
	{
		blob->m_lpKey[i] = Enumerable::ElementAt(coll, i);
	}

	return blob;
}
