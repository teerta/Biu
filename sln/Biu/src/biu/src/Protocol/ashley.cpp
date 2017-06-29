
#include "stdafx.h"
#include "bashley.h"
#include "bmarco.h"
#include "bbase.h"
#include "basobj.h"
#include "biosecc.h"
#include "bcp.h"
#include "bp4.h"
#include "bp4s.h"
#include "bsec.h"

#if defined(__linux__) || defined(__APPLE__)
#include <arpa/inet.h>
#endif


using namespace Biu;


// BAshleyP

BAshley::BAshley()
{
}

BAshley::~BAshley()
{
}

B_Data_Channel_Result BAshley::ParseData(BMemoryBrick* pBrick, size_t& nDataPos, void* pSec, BProtocolRestriction* pRestriction)
{
	assert(pBrick);
	
	BSerializer::Read_Function func = m_bmsRecv.GetReadFunc();
	size_t cbData = pBrick->UntilGetDataSize();    
	size_t nPos = nDataPos;
	size_t nPos2 = nDataPos;
	BIOSecurityContext* pSecurity = reinterpret_cast<BIOSecurityContext*>(pSec);
	BSecurityItem* pSecItem = NULL;
	if (pSecurity)
	{
		pSecItem = pSecurity->GetSecurityItemStatic();
	}
	
	// Peocess header
	if (!func(pBrick, reinterpret_cast<char*>(&m_hdrRecv.Base), B_Ashley_Size_Of_BaseHeader, nPos))
	{
		return B_DC_R_Continue;
	}

	m_hdrRecv.Base.Magic = ntohl(m_hdrRecv.Base.Magic);
	m_hdrRecv.Base.Version = ntohl(m_hdrRecv.Base.Version);
	m_hdrRecv.Base.Size = ntohl(m_hdrRecv.Base.Size);
	m_hdrRecv.Base.Flag = ntohl(m_hdrRecv.Base.Flag);	
	
	if (B_Ashley_Magic != m_hdrRecv.Base.Magic)
	{
		return B_DC_R_Failed;
	}

	// Check all verions here
	if (
		B_ASHLEY_V_1_0 != m_hdrRecv.Base.Version && 
		B_ASHLEY_V_1_1 != m_hdrRecv.Base.Version
		)
	{
		return B_DC_R_Failed;
	}

	if (!m_hdrRecv.Base.Size)
	{
		return B_DC_R_Failed;
	}

	if (m_hdrRecv.Base.Size > cbData)
	{
		return B_DC_R_Continue;
	}

	if (B_ASHLEY_FL_SECURITY & m_hdrRecv.Base.Flag)
	{
		// Security

		short sVal = 0;
		if (!func(pBrick, reinterpret_cast<char*>(&sVal), B_Size_Of_WORD, nPos))
		{
			return B_DC_R_Failed;
		}

		sVal = ntohs(sVal);
		switch (sVal)
		{
		case B_ASHLEY_SEC_P4_1:
		{
			if (!pSecurity)
			{
				return B_DC_R_Failed;
			}

			if (!func(pBrick, reinterpret_cast<char*>(&sVal), B_Size_Of_WORD, nPos))
			{
				return B_DC_R_Failed;
			}

			DWORD cbHash = ntohs(sVal);
			LPBYTE lpHash = (LPBYTE)malloc(cbHash);
			if (!func(pBrick, reinterpret_cast<char*>(lpHash), cbHash, nPos))
			{
				free(lpHash);
				return B_DC_R_Failed;
			}

			if (!func(pBrick, reinterpret_cast<char*>(&sVal), B_Size_Of_WORD, nPos))
			{
				free(lpHash);
				return B_DC_R_Failed;
			}

			DWORD cbSig = ntohs(sVal);
			LPBYTE lpSig = (LPBYTE)malloc(cbSig);
			if (!func(pBrick, reinterpret_cast<char*>(lpSig), cbSig, nPos))
			{
				free(lpHash);
				free(lpSig);
				return B_DC_R_Failed;
			}

			if (cbData > m_hdrRecv.Base.Size)
			{
				nPos2 += m_hdrRecv.Base.Size;
				
			}
			else
			{
				nPos2 = nPos;
			}

			auto* p4 = pSecItem->GetAlgP4();
			BP4PARAM param{ m_bmsRecv.GetShack(), pBrick, pSecurity->GetRemoteKey(), pSecItem->GetKey(), nPos, nPos2, lpSig, cbSig, lpHash, cbHash, 0 };
			if (!p4->Decrypt(&param))
			{
				free(lpHash);
				free(lpSig);
				return B_DC_R_DecryptFailed;
			}

			free(lpHash);
			free(lpSig);
			m_hdrRecv.Base.Size = nPos + param.cbCrypt;
			cbData = pBrick->UntilGetDataSize();
		}
			break;

		case B_ASHLEY_SEC_P4S:
		{
			if (!pSecurity)
			{
				return B_DC_R_Failed;
			}

			if (!func(pBrick, reinterpret_cast<char*>(&sVal), B_Size_Of_WORD, nPos))
			{
				return B_DC_R_Failed;
			}

			DWORD cbHash = ntohs(sVal);
			LPBYTE lpHash = (LPBYTE)malloc(cbHash);
			if (!func(pBrick, reinterpret_cast<char*>(lpHash), cbHash, nPos))
			{
				free(lpHash);
				return B_DC_R_Failed;
			}

			if (!func(pBrick, reinterpret_cast<char*>(&sVal), B_Size_Of_WORD, nPos))
			{
				free(lpHash);
				return B_DC_R_Failed;
			}

			DWORD cbSig = ntohs(sVal);
			LPBYTE lpSig = (LPBYTE)malloc(cbSig);
			if (!func(pBrick, reinterpret_cast<char*>(lpSig), cbSig, nPos))
			{
				free(lpHash);
				free(lpSig);
				return B_DC_R_Failed;
			}

			if (!func(pBrick, reinterpret_cast<char*>(&sVal), B_Size_Of_WORD, nPos))
			{
				free(lpHash);
				free(lpSig);
				return B_DC_R_Failed;
			}

			DWORD cbKey = ntohs(sVal);
			LPBYTE lpKey = (LPBYTE)malloc(cbKey);
			if (!func(pBrick, reinterpret_cast<char*>(lpKey), cbKey, nPos))
			{
				free(lpHash);
				free(lpSig);
				free(lpKey);
				return B_DC_R_Failed;
			}

			if (cbData > m_hdrRecv.Base.Size)
			{
				nPos2 += m_hdrRecv.Base.Size;

			}
			else
			{
				nPos2 = nPos;
			}

			auto* p4s = pSecItem->GetAlgP4S();
			BP4SPARAM param{ m_bmsRecv.GetShack(), pBrick, pSecurity->GetRemoteKey(), pSecItem->GetKey(), nPos, nPos2, 
				lpSig, cbSig, lpHash, cbHash, lpKey, cbKey, 0 };

			if (!p4s->Decrypt(&param))
			{
				free(lpHash);
				free(lpSig);
				free(lpKey);
				return B_DC_R_DecryptFailed;
			}

			free(lpHash);
			free(lpSig);
			free(lpKey);
			m_hdrRecv.Base.Size = nPos + param.cbCrypt;
			cbData = pBrick->UntilGetDataSize();
		}
			break;

		default:
			return B_DC_R_InvalidSecurity;
			break;
		}
	}
	
	size_t cbPacket = 0;

	// Process ExHeader
	//if (B_ASHLEY_FL_FLAGEX & m_hdrRecv.Base.Flag)
	//{
	//	// FlagEx (Dummy now)
	//}

	if (B_ASHLEY_FL_RECOG & m_hdrRecv.Base.Flag)
	{
		// Recog

		if (!func(pBrick, reinterpret_cast<char*>(&m_hdrRecv.Recog), B_Ashley_Size_Of_Recog, nPos))
		{
			return B_DC_R_Continue;
		}

		m_hdrRecv.Recog.Identification = ntohll(m_hdrRecv.Recog.Identification);
		m_hdrRecv.Recog.Processing = ntohll(m_hdrRecv.Recog.Processing);
		m_hdrRecv.Recog.ExHeaderSize = ntohs(m_hdrRecv.Recog.ExHeaderSize);

		cbPacket = m_hdrRecv.Base.Size - m_hdrRecv.Recog.ExHeaderSize - B_Ashley_Size_Of_BaseHeader;
	}

	//if (B_ASHLEY_FL_ACK & m_hdrRecv.Base.Flag)
	//{
	//	// Ack (Dummy now)
	//}

	//if (B_ASHLEY_FL_VERIFICATION & m_hdrRecv.Base.Flag)
	//{
	//	// Verification (Dummy now)
	//}

	if (B_ASHLEY_FL_PART & m_hdrRecv.Base.Flag)
	{
		// Part
		
		if (!func(pBrick, reinterpret_cast<char*>(&m_hdrRecv.Pa), B_Ashley_Size_Of_Part, nPos))
		{
			return B_DC_R_Failed;
		}

		m_hdrRecv.Pa.ID = ntohll(m_hdrRecv.Pa.ID);
		m_hdrRecv.Pa.Total = ntohll(m_hdrRecv.Pa.Total);
		m_hdrRecv.Pa.Current = ntohll(m_hdrRecv.Pa.Current);
	}

	//if (B_ASHLEY_FL_DISPLACEMENT & m_hdrRecv.Base.Flag)
	//{
	//	// Displacement (Dummy now)
	//}

	//if (B_ASHLEY_FL_FILE & m_hdrRecv.Base.Flag)
	//{
	//	// File

	//	if (!func(pBrick, reinterpret_cast<char*>(&m_hdrRecv.File), B_Ashley_Size_Of_File, nPos))
	//	{
	//		return B_DC_R_Failed;
	//	}

	//	m_hdrRecv.File.Size = ntohll(m_hdrRecv.File.Size);
	//	m_hdrRecv.File.ID = ntohl(m_hdrRecv.File.ID);
	//}

	// Check restriction
	if (pRestriction)
	{
		if (pRestriction->GetIdentification() && m_hdrRecv.Recog.Identification != pRestriction->GetIdentification())
		{
			return B_DC_R_Failed;
		}

		if (pRestriction->GetMaxPacketSize() && cbPacket > pRestriction->GetMaxPacketSize())
		{
			return B_DC_R_Failed;
		}

		if (pRestriction->GetMinPacketSize() && cbPacket < pRestriction->GetMinPacketSize())
		{
			return B_DC_R_Failed;
		}
	}
	
	m_bmsRecv.SetBrick(pBrick, nPos);
	
	nDataPos = nPos + cbPacket;
	if (cbData - nPos > cbPacket)
	{
		//nDataPos = nPos + cbPacket;
		return B_DC_R_More;
	}
	
	return B_DC_R_Succeeded;
}

bool BAshley::CreateObj(BMemoryBrick* pBrick, IBSerializable* pObj, void* pVal, void* pSec)
{
	assert(pBrick);
	assert(pVal);
	
	BIOSecurityContext* pSecurity = reinterpret_cast<BIOSecurityContext*>(pSec);
	BSecurityItem* pSecItem = NULL;
	if (pSecurity)
	{
		pSecItem = pSecurity->GetSecurityItemStatic();
	}

	B_ASHLEY_HEADER header;
	LPB_ASHLEY_HEADER lpHeader = &header;
	
	memcpy(lpHeader, pVal, B_Ashley_Size_Of_AshleyHeader);
	
	lpHeader->Base.Size = 0;
	lpHeader->Base.Magic = B_Ashley_Magic;

	// Log or sniff header here if app need diagnostic.
	/*
			Till now header's ExHeaderSize is invalid.
			So you can't dump this data.
	*/

	unsigned int dwFlag = lpHeader->Base.Flag;

	lpHeader->Base.Magic = htonl(lpHeader->Base.Magic);
	lpHeader->Base.Version = htonl(lpHeader->Base.Version);
	lpHeader->Base.Flag = htonl(lpHeader->Base.Flag);	

	unsigned int cbHeader = B_Ashley_Size_Of_BaseHeader;
//    if (B_ASHLEY_FL_FLAGEX & dwFlag)
//    {
//            cbHeader += B_Ashley_Size_Of_FlagEx;
//    }
//    if (B_ASHLEY_FL_ACK & dwFlag)
//    {
//            cbHeader += B_Ashley_Size_Of_Ack;
//    }
//    if (B_ASHLEY_FL_VERIFICATION & dwFlag)
//    {
//            cbHeader += B_Ashley_Size_Of_Verification;
//    }
//    if (B_ASHLEY_FL_SECURITY & dwFlag)
//    {
//            cbHeader += B_Ashley_Size_Of_Security;
//    }
	
	unsigned short usSecType = lpHeader->Sec.Type;
	if (B_ASHLEY_FL_SECURITY & dwFlag)
	{
		lpHeader->Sec.Type = htons(lpHeader->Sec.Type);
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
		lpHeader->Recog.Identification = htonll(lpHeader->Recog.Identification);
		lpHeader->Recog.Processing = htonll(lpHeader->Recog.Processing);

		cbHeader += B_Ashley_Size_Of_Recog;
	}

	if (B_ASHLEY_FL_PART & dwFlag)
	{
		lpHeader->Pa.ID = htonll(lpHeader->Pa.ID);
		lpHeader->Pa.Total = htonll(lpHeader->Pa.Total);
		lpHeader->Pa.Current = htonll(lpHeader->Pa.Current);
			
		cbHeader += B_Ashley_Size_Of_Part;
	}
//    if (B_ASHLEY_FL_DISPLACEMENT & dwFlag)
//    {
//            cbHeader += B_Ashley_Size_Of_Displacement;
//    }

	//if (B_ASHLEY_FL_FILE & dwFlag)
	//{
	//	lpHeader->File.Size = htonll(lpHeader->File.Size);
	//	lpHeader->File.ID = htonl(lpHeader->File.ID);

	//	cbHeader += B_Ashley_Size_Of_File;
	//}
	
	
	/*
	 * Reverse memory for header
	 * Write object
	 * Get serialized data size from brick
	 * Set actual size for header (header + data)
	 * Write base header
	 * Write optional headers
	 */
	auto* pShack = m_bmsSend.GetShack();
	BReserveBrick(pBrick, pShack, cbHeader);
	m_bmsSend.SetBrick(pBrick, cbHeader);
	Biu_DataHeader* pDataHeader = NULL;
	if (pObj)
	{
		pDataHeader = dynamic_cast<Biu_DataHeader*>(pObj);
		if (!pDataHeader)
		{
			CHK_false(pObj->BSerialize(true, m_bmsSend));
		}		
	}
	
	// Base header
	if (pDataHeader)
	{
		lpHeader->Base.Size = htonl(min<unsigned int>((unsigned int)pDataHeader->m_BufferSize, pDataHeader->m_Current + cbHeader));
	}
	else
	{
		lpHeader->Base.Size = htonl(pBrick->UntilGetDataSize());
	}
	
	if (B_ASHLEY_FL_RECOG & dwFlag)
	{
		lpHeader->Recog.ExHeaderSize = htons((unsigned short)cbHeader - (unsigned short)B_Ashley_Size_Of_BaseHeader);
	}
	
	cbHeader = 0;
	BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->Base), B_Ashley_Size_Of_BaseHeader);
	cbHeader += B_Ashley_Size_Of_BaseHeader;

//    // FlagEx (Dummy now)
//    if (B_ASHLEY_FL_FLAGEX & dwFlag)
//    {        
//    }
//
//    // Ack (Dummy now)
//    if (B_ASHLEY_FL_ACK & dwFlag)
//    {
//    }
//
//    // Verification (Dummy now)
//    if (B_ASHLEY_FL_VERIFICATION & dwFlag)
//    {
//    }
//
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

	//// File
	//if (B_ASHLEY_FL_FILE & dwFlag)
	//{
	//	BWriteReservedBrick(pBrick, pShack, cbHeader, reinterpret_cast<char*>(&lpHeader->File), B_Ashley_Size_Of_Part);
	//	cbHeader += B_Ashley_Size_Of_File;
	//}

//     // Displacement (Dummy now)
//    if (B_ASHLEY_FL_DISPLACEMENT & dwFlag)
//    {
//    }


	// Encrypt Data
	if (B_ASHLEY_FL_SECURITY & dwFlag)
	{
		switch (usSecType)
		{
		case B_ASHLEY_SEC_P4_1:
		{
			auto* p4 = pSecItem->GetAlgP4();			
			
			BP4PARAM param{ m_bmsSend.GetShack(), pBrick, NULL, pSecurity->GetRemoteKey(), dwSecurityPos, dwSecurityPos, 0, 0, 0, 0, 0 };
			CHK_false(pSecItem->GetAlgP4()->Encrypt(&param));

			DWORD cbNewSize = htonl(dwSecurityPos + param.cbCrypt);
			BWriteReservedBrick(pBrick, pShack, B_Ashley_Pos_Base_Size, reinterpret_cast<char*>(&cbNewSize), B_Ashley_Size_Of_Type32);
			BWriteReservedBrick(pBrick, pShack, dwSecHashPos, reinterpret_cast<char*>(param.lpHash), p4->GetHashLen());
			BWriteReservedBrick(pBrick, pShack, dwSecSigPos, reinterpret_cast<char*>(param.lpSig), p4->GetSigLen());

			SAFE_FREE(param.lpHash);
			SAFE_FREE(param.lpSig);
		}
			break;

		case B_ASHLEY_SEC_P4S:
		{
			auto* p4s = pSecItem->GetAlgP4S();

			BP4SPARAM param{ m_bmsSend.GetShack(), pBrick, NULL, pSecurity->GetRemoteKey(), dwSecurityPos, dwSecurityPos, 0, 0, 0, 0, 0, 0, 0 };
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

bool BAshley::GetObj(IBSerializable* pObj)
{
	assert(pObj);
	
	return pObj->BSerialize(false, m_bmsRecv);
}

void BAshley::CreatePod(BMemoryBrick* pBrick, const void* pData, size_t cbData)
{
	throw runtime_error("Not Implemented");
}

bool BAshley::GetPod(void* pData, size_t& cbRead)
{
	throw runtime_error("Not Implemented");
	return false;
}

size_t BAshley::GetPodSize()
{
	throw runtime_error("Not Implemented");
	return 0;
}

IBDataChannel* BAshley::Clone() 
{ 
	BAshley* p = new BAshley();
	p->SetMemoryModel(m_bmsSend.GetShack(), m_bmsSend.GetReadFunc());

	return p;
}

void BAshley::ChangeMemory(BMemoryBrick* pBrick)
{
	assert(pBrick);
	
	m_bmsRecv.ChangeBrick(pBrick);
}

void BAshley::SetMemoryModel(BMemoryShack_char* pShack, BSerializer::Read_Function func)
{
	m_bmsSend.SetShack(pShack);
	m_bmsSend.SetReadFunc(func);

	m_bmsRecv.SetShack(pShack);
	m_bmsRecv.SetReadFunc(func);
}

BSerializer::Read_Function BAshley::GetReadFunc()
{
	return m_bmsRecv.GetReadFunc();
}
