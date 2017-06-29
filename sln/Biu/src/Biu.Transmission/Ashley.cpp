
#include "Stdafx.h"
#include "Ashley.h"
#include "bmshack.h"
#include "bp4.h"
#include "bp4s.h"


using namespace Biu::Transmission;


// Ashley

Ashley::Ashley()
	: m_pAshley(NULL)
	, m_obj(nullptr)
	, m_bEntity(false)
{
}

Ashley::~Ashley()
{
	m_obj = nullptr;
	this->!Ashley();
}

Ashley::!Ashley()
{
	if (m_bEntity)
	{
		SAFE_DELETE(m_pAshley);
	}
}

bool Ashley::GetObject(IBiuSerializable^ obj)
{
	try
	{
		return obj->BDeserialize(m_sr);
	}
	catch (Exception^)
	{
		throw;
	}
}

bool Ashley::CreateObject(MemoryBrick^ brick, IBiuSerializable^ obj, AshleyHeader^ header, IOSecurityContext^ sec)
{
	BMemoryBrick* pBrick = brick->Brick;

	BIOSecurityContext* pSecurity = NULL;
	BSecurityItem* pSecItem = NULL;
	if (nullptr != sec)
	{
		pSecurity = reinterpret_cast<BIOSecurityContext*>(sec->m_pInner);
		if (pSecurity)
		{
			pSecItem = pSecurity->GetSecurityItemStatic();
		}
	}

	B_ASHLEY_HEADER nativeHeader;
	LPB_ASHLEY_HEADER lpHeader = &nativeHeader;
	ZeroMemory(lpHeader, B_Ashley_Size_Of_AshleyHeader);

	unsigned int dwFlag = static_cast<unsigned int>(header->Base->Flag);
	lpHeader->Base.Size = 0;
	lpHeader->Base.Magic = htonl(B_Ashley_Magic);
	lpHeader->Base.Version = htonl(static_cast<unsigned int>(header->Base->Version));
	lpHeader->Base.Flag = htonl(dwFlag);

	unsigned int cbHeader = B_Ashley_Size_Of_BaseHeader;

	unsigned short usSecType = (unsigned short)header->Security->Alg;
	if (B_ASHLEY_FL_SECURITY & dwFlag)
	{
		lpHeader->Sec.Type = htons(usSecType);
		DWORD cbSecurity = B_Ashley_Size_Of_Security_Base;
		switch (usSecType)
		{
		case B_ASHLEY_SEC_P4_1:
		{
			if (!pSecurity)
			{
				return false;
			}

			auto* p4 = pSecItem->GetAlgP4();
			lpHeader->Sec.P4.HashSize = htons((short)p4->GetHashLen());
			lpHeader->Sec.P4.SigSize = htons((short)p4->GetSigLen());

			cbSecurity += B_Ashley_Size_Of_Security_P4 + p4->GetHashLen() + p4->GetSigLen();
		}
			break;

		case B_ASHLEY_SEC_P4S:
		{
			if (!pSecurity)
			{
				return false;
			}

			auto* p4s = pSecItem->GetAlgP4S();
			lpHeader->Sec.P4S.HashSize = htons((short)p4s->GetHashLen());
			lpHeader->Sec.P4S.SigSize = htons((short)p4s->GetSigLen());
			lpHeader->Sec.P4S.KeySize = htons((short)p4s->GetKeyLen());

			cbSecurity += B_Ashley_Size_Of_Security_P4S + p4s->GetHashLen() + p4s->GetSigLen() + p4s->GetKeyLen();
		}
		break;

		default:
			return false;
			break;
		}

		cbHeader += cbSecurity;
	}

	if (B_ASHLEY_FL_RECOG & dwFlag)
	{
		lpHeader->Recog.Identification = htonll(header->Recog->Identification);
		lpHeader->Recog.Processing = htonll(header->Recog->Processing);

		cbHeader += B_Ashley_Size_Of_Recog;
	}

	if (B_ASHLEY_FL_PART & dwFlag)
	{
		lpHeader->Pa.ID = htonll(header->Part->ID);
		lpHeader->Pa.Total = htonll(header->Part->Total);
		lpHeader->Pa.Current = htonll(header->Part->Current);

		cbHeader += B_Ashley_Size_Of_Part;
	}


	auto* pSer = m_ss->Serializer;
	auto* pShack = pSer->GetShack();
	BReserveBrick(pBrick, pSer->GetShack(), cbHeader);
	pSer->SetBrick(pBrick, cbHeader);
	if (nullptr != obj)
	{
		CHK_false(obj->BSerialize(m_ss));
	}
	
	lpHeader->Base.Size = htonl(pBrick->UntilGetDataSize());

	if (B_ASHLEY_FL_RECOG & dwFlag)
	{
		lpHeader->Recog.ExHeaderSize = htons((unsigned short)cbHeader - (unsigned short)B_Ashley_Size_Of_BaseHeader);
	}
	
	cbHeader = 0;
	BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Base), B_Ashley_Size_Of_BaseHeader);
	cbHeader += B_Ashley_Size_Of_BaseHeader;

	// Security
	DWORD dwSecHashPos = 0;
	DWORD dwSecSigPos = 0;
	DWORD dwSecKeyPos = 0;
	if (B_ASHLEY_FL_SECURITY & dwFlag)
	{
		switch (usSecType)
		{
		case B_ASHLEY_SEC_P4_1:
		{
			auto* p4 = pSecItem->GetAlgP4();
			BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Sec.Type), B_Size_Of_WORD);
			cbHeader += B_Size_Of_WORD;

			BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Sec.P4.HashSize), B_Size_Of_WORD);
			cbHeader += B_Size_Of_WORD;

			dwSecHashPos = cbHeader;
			cbHeader += p4->GetHashLen();

			BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Sec.P4.SigSize), B_Size_Of_WORD);
			cbHeader += B_Size_Of_WORD;

			dwSecSigPos = cbHeader;
			cbHeader += p4->GetSigLen();
		}
			break;

		case B_ASHLEY_SEC_P4S:
		{
			auto* p4s = pSecItem->GetAlgP4S();
			BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Sec.Type), B_Size_Of_WORD);
			cbHeader += B_Size_Of_WORD;

			BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Sec.P4S.HashSize), B_Size_Of_WORD);
			cbHeader += B_Size_Of_WORD;

			dwSecHashPos = cbHeader;
			cbHeader += p4s->GetHashLen();

			BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Sec.P4S.SigSize), B_Size_Of_WORD);
			cbHeader += B_Size_Of_WORD;

			dwSecSigPos = cbHeader;
			cbHeader += p4s->GetSigLen();

			BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Sec.P4S.KeySize), B_Size_Of_WORD);
			cbHeader += B_Size_Of_WORD;

			dwSecKeyPos = cbHeader;
			cbHeader += p4s->GetKeyLen();
		}
		break;

		default:
			return false;
			break;
		}
	}

	DWORD dwSecurityPos = cbHeader;

	// Recog	
	if (B_ASHLEY_FL_RECOG & dwFlag)
	{
		BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Recog), B_Ashley_Size_Of_Recog);
		cbHeader += B_Ashley_Size_Of_Recog;
	}

	// Part
	if (B_ASHLEY_FL_PART & dwFlag)
	{
		BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Pa), B_Ashley_Size_Of_Part);
		cbHeader += B_Ashley_Size_Of_Part;
	}

	// Encrypt Data
	if (B_ASHLEY_FL_SECURITY & dwFlag)
	{
		switch (usSecType)
		{
		case B_ASHLEY_SEC_P4_1:
		{
			auto* p4 = pSecItem->GetAlgP4();

			BP4PARAM param{ pShack, pBrick, NULL, pSecurity->GetRemoteKey(), dwSecurityPos, dwSecurityPos, 0, 0, 0, 0, 0 };
			CHK_false(pSecItem->GetAlgP4()->Encrypt(&param));

			DWORD cbNewSize = htonl(dwSecurityPos + param.cbCrypt);
			BWriteReservedBrick(pBrick, pShack, B_Ashley_Pos_Base_Size, reinterpret_cast<char*>(&cbNewSize), B_Ashley_Size_Of_Type32);
			BWriteReservedBrick(pBrick, pShack, dwSecHashPos, reinterpret_cast<char*>(param.lpHash), p4->GetHashLen());
			BWriteReservedBrick(pBrick, pShack, dwSecSigPos, reinterpret_cast<char*>(param.lpSig), p4->GetSigLen());

			SAFE_DELETE_ARRAY(param.lpHash);
			SAFE_DELETE_ARRAY(param.lpSig);
		}
			break;

		case B_ASHLEY_SEC_P4S:
		{
			auto* p4s = pSecItem->GetAlgP4S();

			BP4SPARAM param{ pShack, pBrick, NULL, pSecurity->GetRemoteKey(), dwSecurityPos, dwSecurityPos, 0, 0, 0, 0, 0, 0, 0 };
			CHK_false(p4s->Encrypt(&param));

			DWORD cbNewSize = htonl(dwSecurityPos + param.cbCrypt);
			BWriteReservedBrick(pBrick, pShack, B_Ashley_Pos_Base_Size, reinterpret_cast<char*>(&cbNewSize), B_Ashley_Size_Of_Type32);
			BWriteReservedBrick(pBrick, pShack, dwSecHashPos, reinterpret_cast<char*>(param.lpHash), p4s->GetHashLen());
			BWriteReservedBrick(pBrick, pShack, dwSecSigPos, reinterpret_cast<char*>(param.lpSig), p4s->GetSigLen());
			BWriteReservedBrick(pBrick, pShack, dwSecKeyPos, reinterpret_cast<char*>(param.lpSesionKey), p4s->GetKeyLen());

			SAFE_FREE(param.lpHash);
			SAFE_FREE(param.lpSig);
			SAFE_FREE(param.lpSesionKey);
		}
		break;

		default:
			return false;
			break;
		}
	}

	pBrick->GetLastBrick()->SetEndOfSeries(true);

	return true;
}

IBiuSerializable^ Ashley::KillObject()
{
	IBiuSerializable^ obj = m_obj;
	m_obj = nullptr;

	return obj;
}

generic<class T>
where T : IBiuSerializable
bool Ashley::GetObjKept(T% obj)
{
	try
	{
		if (nullptr != m_obj)
		{
			if (m_obj->GetType() != T::typeid)
			{
				return false;
			}

			obj = safe_cast<T>(m_obj);
			return true;
		}

		bool bRet = obj->BDeserialize(m_sr);
		if (bRet)
		{
			m_obj = obj;
		}

		return bRet;
	}
	catch (Exception^)
	{
		throw;
	}
}

int Ashley::GetReceivedRaw(MemoryBrick^% brick, unsigned long long% pos)
{
	brick = gcnew MemoryBrick();
	brick->Brick = m_pAshley->GetRecvSerializer()->GetBrick();
	pos = m_pAshley->GetRecvSerializer()->GetReadPos();

	return (int)m_pAshley->GetRecvHeader()->Pa.ID;
}

Ashley^ Ashley::CreateEntityProtocol()
{
	BAshley* p = new BAshley();
	Ashley^ ashley = gcnew Ashley();
	ashley->m_bEntity = true;
	ashley->AshleyProtocol = p;

	return ashley;
}

AshleyHeader^ Ashley::CreateHeader(AshleyVersion version, unsigned long long identification,
	unsigned long long processing, AshleyFlag flag)
{
	AshleyHeader^ header = gcnew AshleyHeader();
	header->Base->Version = version;	
	header->Base->Flag = (unsigned int)(flag | AshleyFlag::Recog);
	header->Recog->Identification = identification;
	header->Recog->Processing = processing;

	return header;
}
