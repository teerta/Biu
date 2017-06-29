#include "stdafx.h"
#include "QuickObject.h"


using namespace Biu::Transmission;
using namespace System::Text;


// CommandBase

CommandBase::CommandBase()
	: CommandVar(0)
	, PartialToken(0)
{
}

bool CommandBase::BSerialize(BiuSerializer^ ser)
{
	bool bRet = true;
	try
	{
		CommandTimestamp = DateTime::Now;

		ser
			->Serialize(CommandTimestamp.ToFileTimeUtc())
			->Serialize(CommandVar)
			->Serialize(PartialToken);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool CommandBase::BDeserialize(BiuSerializer^ ser)
{
	bool bRet = true;
	try
	{
		long long ll = 0;
		ser
			->Deserialize(ll)
			->Deserialize(CommandVar)
			->Deserialize(PartialToken);

		CommandTimestamp = DateTime::FromFileTimeUtc(ll);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// Ack

Ack::Ack()
	: CommandBase()
	, AckAction(0)
	, AckResult(0)
	, ErrorCode(0)
{
}

bool Ack::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(AckAction)
			->Serialize(AckResult)
			->Serialize(ErrorCode);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool Ack::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(AckAction)
			->Deserialize(AckResult)
			->Deserialize(ErrorCode);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// AckMessage

AckMessage::AckMessage()
	: Ack()
{
	ErrorTitle = String::Empty;
	ErrorMessage = String::Empty;
	ErrorDetail = String::Empty;
	ResolveUrl = String::Empty;
}

bool AckMessage::BSerialize(BiuSerializer^ ser)
{
	CHK_false(Ack::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(ErrorTitle)
			->Serialize(ErrorMessage)
			->Serialize(ErrorDetail)
			->Serialize(ResolveUrl);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool AckMessage::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(Ack::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(ErrorTitle)
			->Deserialize(ErrorMessage)
			->Deserialize(ErrorDetail)
			->Deserialize(ResolveUrl);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// Hello

Hello::Hello()
	: CommandBase()
	, Flag(0)
{
}

bool Hello::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(Title)
			->Serialize(Message)
			->Serialize(Flag);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool Hello::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(Title)
			->Deserialize(Message)
			->Deserialize(Flag);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// SendFile

SendFile::SendFile()
	: CommandBase()
	, Size(0)
	, ID(0)
	, OpMode(BiuOpMode::None)
{
}

bool SendFile::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(ID)
			->Serialize(Size)
			->Serialize((int)OpMode)
			->Serialize(File)
			->Serialize(Key)
			->Serialize(Hash);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool SendFile::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		int nOpMode = 0;

		ser
			->Deserialize(ID)
			->Deserialize(Size)
			->Deserialize(nOpMode)
			->Deserialize(File)
			->Deserialize(Key)
			->Deserialize(Hash);

		OpMode = (BiuOpMode)nOpMode;
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// RecvFile

RecvFile::RecvFile()
	: CommandBase()
	, ID(0)
	, Origin(0)
	, Size(0)
{
}

bool RecvFile::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(ID)
			->Serialize(Origin)
			->Serialize(Size);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool RecvFile::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(ID)
			->Deserialize(Origin)
			->Deserialize(Size);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// AskFile

AskFile::AskFile()
	: CommandBase()
{
}

bool AskFile::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(File)
			->Serialize(Key);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool AskFile::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(File)
			->Deserialize(Key);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// ReplyFile

ReplyFile::ReplyFile()
	: AckMessage()
{
}

bool ReplyFile::BSerialize(BiuSerializer^ ser)
{
	CHK_false(AckMessage::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(ID)
			->Serialize(File)
			->Serialize(Key);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool ReplyFile::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(AckMessage::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(ID)
			->Deserialize(File)
			->Deserialize(Key);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// PurifyFile

PurifyFile::PurifyFile()
	: CommandBase()
	, OpMode(BiuOpMode::None)
	, Size(0)	
{
}

bool PurifyFile::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize((int)OpMode)
			->Serialize(Size)			
			->Serialize(File)
			->Serialize(Key)
			->Serialize(Hash);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool PurifyFile::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		int nOpMode = 0;

		ser
			->Deserialize(nOpMode)
			->Deserialize(Size)			
			->Deserialize(File)
			->Deserialize(Key)
			->Deserialize(Hash);

		OpMode = (BiuOpMode)nOpMode;
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// RawData

RawData::RawData()
	: CommandBase()
{
}

bool RawData::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->SerializeBuffer(Raw);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool RawData::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		Raw = gcnew List<byte>();
		ser
			->DeserializeBuffer(Raw);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}


// Token

Token::Token()
	: CommandBase()
{
	this->TokenStamp = Guid::NewGuid().ToString();
}

bool Token::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(TokenStamp)
			->Serialize(TokenContent);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool Token::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(TokenStamp)
			->Deserialize(TokenContent);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

String^ Token::FromParams(cli::array<String^>^ params, String^ sep)
{
	StringBuilder^ sb = gcnew StringBuilder();
	for each (String^ var in params)
	{
		sb->Append(var)->Append(sep);
	}

	return sb->Remove(sb->Length - sep->Length, sep->Length)->ToString();
}

cli::array<String^>^ Token::ToParams(String^ token, String^ sep)
{
	cli::array<String^>^ seps = gcnew cli::array<String^>(1);
	seps[0] = sep;

	return token->Split(seps, StringSplitOptions::None);
}


// Login

Login::Login()
	: CommandBase()
{
}

bool Login::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(UserName)
			->Serialize(Password);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool Login::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(UserName)
			->Deserialize(Password);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool Login::IsValid(int minUserLen, int minPasswordLen, cli::array<String^>^ invalidStrings)
{
	if (minUserLen <= 0)
	{
		throw gcnew ArgumentException("minUserLen should  greater than zero.");
	}

	if (minPasswordLen <= 0)
	{
		throw gcnew ArgumentException("minPasswordLen should greater than zero.");
	}

	if (String::IsNullOrEmpty(UserName) || String::IsNullOrEmpty(Password))
	{
		return false;
	}

	if (UserName->Length < minUserLen || Password->Length < minPasswordLen)
	{
		return false;
	}

	if (nullptr != invalidStrings)
	{
		if (-1 != Array::IndexOf(invalidStrings, UserName) || -1 != Array::IndexOf(invalidStrings, Password))
		{
			return false;
		}
	}

	return true;
}


// ChangePassword

ChangePassword::ChangePassword()
	: CommandBase()
{
}

bool ChangePassword::BSerialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BSerialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Serialize(UserName)
			->Serialize(NewPassword)
			->Serialize(OldPassword);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool ChangePassword::BDeserialize(BiuSerializer^ ser)
{
	CHK_false(CommandBase::BDeserialize(ser));

	bool bRet = true;
	try
	{
		ser
			->Deserialize(UserName)
			->Deserialize(NewPassword)
			->Deserialize(OldPassword);
	}
	catch (Exception^)
	{
		bRet = false;
	}

	return bRet;
}

bool ChangePassword::IsValid(bool all, int minUserLen, int minPasswordLen, cli::array<String^>^ invalidStrings)
{
	if (minUserLen <= 0)
	{
		throw gcnew ArgumentException("minUserLen should  greater than zero.");
	}

	if (minPasswordLen <= 0)
	{
		throw gcnew ArgumentException("minPasswordLen should greater than zero.");
	}

	if (String::IsNullOrEmpty(UserName) || String::IsNullOrEmpty(NewPassword))
	{
		return false;
	}

	if (UserName->Length < minUserLen || NewPassword->Length < minPasswordLen)
	{
		return false;
	}

	if (nullptr != invalidStrings)
	{
		if (-1 != Array::IndexOf(invalidStrings, UserName) || -1 != Array::IndexOf(invalidStrings, NewPassword))
		{
			return false;
		}
	}

	if (all)
	{
		if (String::IsNullOrEmpty(OldPassword) || OldPassword->Length < minPasswordLen)
		{
			return false;
		}

		if (nullptr != invalidStrings && -1 != Array::IndexOf(invalidStrings, OldPassword))
		{
			return false;
		}
	}

	return true;
}
