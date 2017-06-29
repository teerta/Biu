
#ifndef _BAQ_H_
#define _BAQ_H_


// Biu Serializable Quick Object


#include "biu.h"
#include "bs.h"
#include "baqdef.h"


namespace Biu {


	// Class Biu_CmdBase

	class BIU_API Biu_CmdBase : public IBSerializable {
	public:
		Biu_CmdBase();
		virtual ~Biu_CmdBase();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		long long m_t;
		long long m_CmdVar;
		int m_PartialToken;
	};


	// Class Biu_Ack

	class BIU_API Biu_Ack : public Biu_CmdBase {
	public:
		Biu_Ack();
		virtual ~Biu_Ack();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		unsigned long long	m_AckAction;
		unsigned int		m_Result;
		long long			m_ErrCode;
	};


	// Class Biu_Ack

	class BIU_API Biu_AckMessage : public Biu_Ack {
	public:
		Biu_AckMessage();
		virtual ~Biu_AckMessage();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		CAtlStringW		m_ErrTitle;
		CAtlStringW		m_ErrMessage;
		CAtlStringW		m_ErrDetail;
		CAtlStringW		m_ResolveUrl;
	};


	// Class Biu_Hello

	class BIU_API Biu_Hello : public Biu_CmdBase {
	public:
		Biu_Hello();
		virtual ~Biu_Hello();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		CAtlStringW m_Title;
		CAtlStringW m_Message;
		unsigned long long m_Flag;
	};


	// Class Biu_SendFile

	class BIU_API Biu_SendFile : public Biu_CmdBase
	{
	public:
		Biu_SendFile();
		virtual ~Biu_SendFile();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		int m_ID;
		unsigned long long m_Size;
		BIU_OPMODE m_OpMode;
		CAtlStringW m_FileName;
		CAtlStringW m_Key;
		CAtlStringW m_Hash;		
	};


	// Class Biu_RecvFile

	class BIU_API Biu_RecvFile : public Biu_CmdBase
	{
	public:
		Biu_RecvFile();
		virtual ~Biu_RecvFile();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		int m_ID;
		unsigned long long m_Origin;
		unsigned long long m_Size;
	};

	// Class Biu_AskFile

	class BIU_API Biu_AskFile : public Biu_CmdBase
	{
	public:
		Biu_AskFile();
		virtual ~Biu_AskFile();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		CAtlStringW m_FileName;
		CAtlStringW m_Key;
	};


	// Class Biu_ReplyFile

	class BIU_API Biu_ReplyFile : public Biu_AckMessage
	{
	public:
		Biu_ReplyFile();
		virtual ~Biu_ReplyFile();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		int m_ID;
		CAtlStringW m_FileName;
		CAtlStringW m_Key;
	};

	// Class Biu_PurifyFile

	class BIU_API Biu_PurifyFile : public Biu_CmdBase
	{
	public:
		Biu_PurifyFile();
		virtual ~Biu_PurifyFile();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		BIU_OPMODE m_OpMode;
		unsigned long long m_Size;		
		CAtlStringW m_FileName;
		CAtlStringW m_Key;
		CAtlStringW m_Hash;
	};


	// Class Biu_RawData

	class BIU_API Biu_RawData : public Biu_CmdBase
	{
	public:
		Biu_RawData();
		virtual ~Biu_RawData();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		unsigned int m_Size;
		unsigned char* m_Data;
	};


	// Class Biu_Token

	class BIU_API Biu_Token : public Biu_CmdBase
	{
	public:
		Biu_Token();
		virtual ~Biu_Token();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		CAtlStringW m_Token;
	};


	// Class Biu_Login

	class BIU_API Biu_Login : public Biu_CmdBase
	{
	public:
		Biu_Login();
		virtual ~Biu_Login();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		CAtlStringW m_UserName;
		CAtlStringW m_Password;
	};


	// Class Biu_ChangePassword

	class BIU_API Biu_ChangePassword : public Biu_CmdBase
	{
	public:
		Biu_ChangePassword();
		virtual ~Biu_ChangePassword();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		CAtlStringW m_UserName;
		CAtlStringW m_NewPassword;
		CAtlStringW m_OldPassword;
	};
}



#endif // _BAQ_H_
