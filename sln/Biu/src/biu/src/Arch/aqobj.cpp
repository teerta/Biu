
#include "stdafx.h"
#include "baq.h"


using namespace Biu;


// Biu_CmdBase

Biu_CmdBase::Biu_CmdBase()
	: m_CmdVar(0)
	, m_PartialToken(0)
{
}

Biu_CmdBase::~Biu_CmdBase()
{
}

bool Biu_CmdBase::BSerialize(bool bStoring, BSerializer& bms)
{
	if (bStoring)
	{
		m_t = time(NULL);

		bms.Serialize(m_t);
		bms.Serialize(m_CmdVar);
		bms.Serialize(m_PartialToken);
	}
	else
	{
		BSBR(bms.Deserialize(m_t));
		BSBR(bms.Deserialize(m_CmdVar));
		BSBR(bms.Deserialize(m_PartialToken));
	}

	return true;
}


// Biu_Ack

Biu_Ack::Biu_Ack()
{
}

Biu_Ack::~Biu_Ack()
{
}

bool Biu_Ack::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring)
	{
		bms.Serialize((unsigned long long)m_AckAction);
		bms.Serialize(m_Result);
		bms.Serialize(m_ErrCode);
	}
	else
	{
		BSBR(bms.Deserialize(reinterpret_cast<unsigned long long&>(m_AckAction)));
		BSBR(bms.Deserialize(reinterpret_cast<unsigned int&>(m_Result)));
		BSBR(bms.Deserialize(m_ErrCode));
	}

	return true;
}


// Biu_AckMessage

Biu_AckMessage::Biu_AckMessage()
{
}

Biu_AckMessage::~Biu_AckMessage()
{
}

bool Biu_AckMessage::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring)
	{
		bms.Serialize(m_ErrTitle);
		bms.Serialize(m_ErrMessage);
		bms.Serialize(m_ErrDetail);
		bms.Serialize(m_ResolveUrl);
	}
	else
	{
		BSBR(bms.Deserialize(m_ErrTitle));
		BSBR(bms.Deserialize(m_ErrMessage));
		BSBR(bms.Deserialize(m_ErrDetail));
		BSBR(bms.Deserialize(m_ResolveUrl));
	}

	return true;
}


// Biu_Hello

Biu_Hello::Biu_Hello()
	: m_Flag(0)
{
}

Biu_Hello::~Biu_Hello()
{
}

bool Biu_Hello::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring)
	{
		bms.Serialize(m_Title);
		bms.Serialize(m_Message);
		bms.Serialize(m_Flag);
	}
	else
	{
		BSBR(bms.Deserialize(m_Title));
		BSBR(bms.Deserialize(m_Message));
		BSBR(bms.Deserialize(m_Flag));
	}

	return true;
}


// Biu_SendFile

Biu_SendFile::Biu_SendFile()
	: Biu_CmdBase() 
	, m_Size(0)
	, m_ID(0)
	, m_OpMode(Biu_OPM_None)
{
}

Biu_SendFile::~Biu_SendFile() 
{
}

bool Biu_SendFile::BSerialize(bool bStoring, BSerializer& bms) 
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring) {
		bms.Serialize(m_ID);
		bms.Serialize(m_Size);		
		bms.Serialize(m_OpMode);
		bms.Serialize(m_FileName);
		bms.Serialize(m_Key);
		bms.Serialize(m_Hash);
	}
	else
	{
		BSBR(bms.Deserialize(m_ID));
		BSBR(bms.Deserialize(m_Size));
		BSBR(bms.Deserialize((int&)*&m_OpMode));
		BSBR(bms.Deserialize(m_FileName));
		BSBR(bms.Deserialize(m_Key));
		BSBR(bms.Deserialize(m_Hash));
	}

	return true;
}


// Biu_RecvFile

Biu_RecvFile::Biu_RecvFile()
	: Biu_CmdBase() 
	, m_ID(0)
	, m_Origin(0)
	, m_Size(0)
{
}

Biu_RecvFile::~Biu_RecvFile() 
{
}

bool Biu_RecvFile::BSerialize(bool bStoring, BSerializer& bms) 
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring) {
		bms.Serialize(m_ID);
		bms.Serialize(m_Origin);
		bms.Serialize(m_Size);
	}
	else
	{
		BSBR(bms.Deserialize(m_ID));
		BSBR(bms.Deserialize(m_Origin));
		BSBR(bms.Deserialize(m_Size));
	}

	return true;
}


// Biu_AskFile

Biu_AskFile::Biu_AskFile()
	: Biu_CmdBase()
{
}

Biu_AskFile::~Biu_AskFile()
{
}

bool Biu_AskFile::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring) {
		bms.Serialize(m_FileName);
		bms.Serialize(m_Key);
	}
	else
	{
		BSBR(bms.Deserialize(m_FileName));
		BSBR(bms.Deserialize(m_Key));
	}

	return true;
}


// Biu_ReplyFile

Biu_ReplyFile::Biu_ReplyFile()
	: Biu_AckMessage()
	, m_ID(0)
{
}

Biu_ReplyFile::~Biu_ReplyFile()
{
}

bool Biu_ReplyFile::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_AckMessage::BSerialize(bStoring, bms));
	if (bStoring) {
		bms.Serialize(m_ID);
		bms.Serialize(m_FileName);
		bms.Serialize(m_Key);
	}
	else
	{
		BSBR(bms.Deserialize(m_ID));
		BSBR(bms.Deserialize(m_FileName));
		BSBR(bms.Deserialize(m_Key));
	}

	return true;
}


// Biu_PurifyFile

Biu_PurifyFile::Biu_PurifyFile()
	: Biu_CmdBase()
	, m_OpMode(Biu_OPM_None)
	, m_Size(0)	
{
}

Biu_PurifyFile::~Biu_PurifyFile()
{
}

bool Biu_PurifyFile::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring) {
		bms.Serialize(m_OpMode);
		bms.Serialize(m_Size);		
		bms.Serialize(m_FileName);
		bms.Serialize(m_Key);
		bms.Serialize(m_Hash);
	}
	else
	{
		BSBR(bms.Deserialize((int&)*&m_OpMode));
		BSBR(bms.Deserialize(m_Size));		
		BSBR(bms.Deserialize(m_FileName));
		BSBR(bms.Deserialize(m_Key));
		BSBR(bms.Deserialize(m_Hash));
	}

	return true;
}


// Biu_RawData

Biu_RawData::Biu_RawData()
	: Biu_CmdBase()
	, m_Size(0)
	, m_Data(0)
{
}

Biu_RawData::~Biu_RawData()
{
	SAFE_DELETE_ARRAY(m_Data);
}

bool Biu_RawData::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring) {
		bms.Serialize(m_Data, m_Size);
	}
	else
	{
		BSBR(bms.Deserialize(&m_Data, (unsigned int*)&m_Size));
	}

	return true;
}


// Biu_Token

Biu_Token::Biu_Token()
	: Biu_CmdBase()
{
}

Biu_Token::~Biu_Token()
{
}

bool Biu_Token::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring)
	{
		bms.Serialize(m_Token);
	}
	else
	{
		BSBR(bms.Deserialize(m_Token));
	}

	return true;
}


// Biu_Login

Biu_Login::Biu_Login()
	: Biu_CmdBase()
{
}

Biu_Login::~Biu_Login()
{
}

bool Biu_Login::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring) 
	{
		bms.Serialize(m_UserName);
		bms.Serialize(m_Password);
	}
	else
	{
		BSBR(bms.Deserialize(m_UserName));
		BSBR(bms.Deserialize(m_Password));
	}

	return true;
}


// Biu_Login

Biu_ChangePassword::Biu_ChangePassword()
	: Biu_CmdBase()
{
}

Biu_ChangePassword::~Biu_ChangePassword()
{
}

bool Biu_ChangePassword::BSerialize(bool bStoring, BSerializer& bms)
{
	BSBR(Biu_CmdBase::BSerialize(bStoring, bms));
	if (bStoring)
	{
		bms.Serialize(m_UserName);
		bms.Serialize(m_NewPassword);
		bms.Serialize(m_OldPassword);
	}
	else
	{
		BSBR(bms.Deserialize(m_UserName));
		BSBR(bms.Deserialize(m_NewPassword));
		BSBR(bms.Deserialize(m_OldPassword));
	}

	return true;
}
