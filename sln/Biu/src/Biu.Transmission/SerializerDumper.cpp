#include "stdafx.h"
#include "SerializerDumper.h"


using namespace System;
using namespace Biu;
using namespace Biu::Transmission;


// BiuSerializationDumper

BiuSerializationDumper::BiuSerializationDumper(MemoryShack^ shack)
	: m_serializer(nullptr)
	, m_pSerializer(NULL)
	, m_pBrick(NULL)
{
	m_pShack = shack->m_pShack;
	m_pBrick = m_pShack->Pop();

	m_pSerializer = new BSerializer();
	m_pSerializer->SetReadFunc(BReadBrickSparsely_char);
	m_pSerializer->SetShack(shack->m_pShack);
	m_pSerializer->SetBrick(m_pBrick);
}

BiuSerializationDumper::~BiuSerializationDumper()
{
	this->!BiuSerializationDumper();
}

BiuSerializationDumper::!BiuSerializationDumper()
{
	SAFE_DELETE(m_pSerializer);
	if (m_pBrick)
	{
		m_pShack->Push(m_pBrick);
		m_pBrick = NULL;
	}
}

generic<class T>
bool BiuSerializationDumper::Dump(Stream^ stream, T obj)
{
	if (nullptr == stream)
	{
		throw gcnew ArgumentNullException("stream is null");
	}

	if (!stream->CanWrite)
	{
		throw gcnew ArgumentException("stream can\'t write");
	}

	if (!stream->CanSeek)
	{
		throw gcnew ArgumentException("stream can\'t seek");
	}

	m_pBrick = m_pShack->Pop();
	m_pSerializer->ResetReadPos();
	m_pSerializer->SetBrick(m_pBrick);

	CHK_false(obj->BSerialize(m_serializer));
	auto* p = m_pBrick;
	while (p)
	{
		for (size_t i = 0; i < p->GetDataSize(); i++)
		{
			stream->WriteByte(*(unsigned char*)p->GetOffsetMemory(i));
		}

		p = p->GetNext();
	}

	return true;
}

generic<class T>
T BiuSerializationDumper::Recycle(Stream^ stream)
{
	return T();
}
