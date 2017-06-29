#pragma once


#include "Serializer.h"
#include "Enum.h"


using namespace System;
using namespace Biu;


namespace Biu { namespace Transmission {


	// Ref Class CommandBase

	public ref class CommandBase : public IBiuSerializable
	{
	public:
		const static unsigned long long ActionBase = 0x00000100;
		const static unsigned int AckBase = 0x00010000;
		
		
		__clrcall CommandBase();

		virtual bool __clrcall BSerialize(BiuSerializer^ ser);
		virtual bool __clrcall BDeserialize(BiuSerializer^ ser);


		DateTime CommandTimestamp;
		long long CommandVar;
		int PartialToken;
	};


	// Ref Class Ack

	public ref class Ack : public CommandBase
	{
	public:
		__clrcall Ack();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		unsigned long long AckAction;
		unsigned int AckResult;
		long long ErrorCode;
	};

	// Ref Class AckMessage

	public ref class AckMessage : public Ack
	{
	public:
		__clrcall AckMessage();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		String^ ErrorTitle;
		String^ ErrorMessage;
		String^ ErrorDetail;
		String^ ResolveUrl;
	};


	// Ref Class Hello

	public ref class Hello : public CommandBase
	{
	public:
		__clrcall Hello();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		String^ Title;
		String^ Message;
		unsigned long long Flag;
	};


	// Ref Class SendFile

	public ref class SendFile : public CommandBase
	{
	public:
		__clrcall SendFile();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		int ID;
		unsigned long long Size;
		BiuOpMode OpMode;
		String^ File;
		String^ Key;
		String^ Hash;
	};


	// Ref Class RecvFile

	public ref class RecvFile : public CommandBase
	{
	public:
		__clrcall RecvFile();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		int ID;
		unsigned long long Origin;
		unsigned long long Size;
		
	};


	// Ref Class AskFile

	public ref class AskFile : public CommandBase
	{
	public:
		__clrcall AskFile();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		String^ File;
		String^ Key;
	};


	// Ref Class ReplyFile

	public ref class ReplyFile : public AckMessage
	{
	public:
		__clrcall ReplyFile();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		int ID;
		String^ File;
		String^ Key;
	};


	// Ref Class PurifyFile

	public ref class PurifyFile : public CommandBase
	{
	public:
		__clrcall PurifyFile();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		BiuOpMode OpMode;
		unsigned long long Size;		
		String^ File;
		String^ Key;
		String^ Hash;
	};


	// Ref Class RawData

	public ref class RawData : public CommandBase
	{
	public:
		__clrcall RawData();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;

		List<byte>^ Raw;
	};


	// Ref Class Token

	public ref class Token : public CommandBase
	{
	public:
		static String^ __clrcall FromParams(cli::array<String^>^ params, String^ sep);
		static cli::array<String^>^ __clrcall ToParams(String^ token, String^ sep);


		__clrcall Token();

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;


		String^ TokenStamp;
		String^ TokenContent;
	};


	// Ref Class Login

	public ref class Login : public CommandBase
	{
	public:
		__clrcall Login();

		bool __clrcall IsValid(int minUserLen, int minPasswordLen, cli::array<String^>^ invalidStrings);

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;

		String^ UserName;
		String^ Password;
	};


	// Ref Class ChangePassword

	public ref class ChangePassword : public CommandBase
	{
	public:
		__clrcall ChangePassword();

		bool __clrcall IsValid(bool all, int minUserLen, int minPasswordLen, cli::array<String^>^ invalidStrings);

		bool __clrcall BSerialize(BiuSerializer^ ser) override;
		bool __clrcall BDeserialize(BiuSerializer^ ser) override;

		String^ UserName;
		String^ NewPassword;
		String^ OldPassword;
	};


} }
