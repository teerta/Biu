#include "stdafx.h"
#include "PeerData.h"
#include "Peer.h"


using namespace Biu::Transmission;


// DataContinuation

DataContinuation::DataContinuation()
{

}

void DataContinuation::Create(BDataContinuation* pContinuation)
{
	this->ID = pContinuation->m_nID;
	this->Direction = pContinuation->m_bDir;
	this->Size = pContinuation->m_cb;
	this->Origin = pContinuation->m_ullOrigin;
	this->FinishedSize = pContinuation->m_ullFinished;
	this->FilePath = gcnew String(CT2W(pContinuation->m_strFile));
	this->PrimaryPeer = gcnew Peer();
	this->PrimaryPeer->InnerPeer = pContinuation->m_primaryPeer;
}


// DataInfo

DataInfo::DataInfo()
	: m_pInner(NULL)
{
}

int DataInfo::ConvertToSendFileID(int recvFileID)
{
	return recvFileID & 0x7fffffff;
}

int DataInfo::ConvertToRecvFileID(int sendFileID)
{
	return sendFileID | 0x80000000;
}


// WaitDataInfo

WaitDataInfo::WaitDataInfo()
	: m_pInner(NULL)
{
}


// DataPeer

DataPeer::DataPeer()
	: m_pInner(NULL)
{
}

DataInfo^ DataPeer::GetDataInfo(int nID)
{
	auto* p = m_pInner->GetDataInfo(nID);
	if (NULL == p)
	{
		return nullptr;
	}

	DataInfo^ di = gcnew DataInfo();
	di->Inner = p;

	return di;
}


// ClientDataPeer

ClientDataPeer::ClientDataPeer()
	: m_pInner(NULL)
{

}

DataInfo^ ClientDataPeer::GetDataInfo(int nID)
{
	auto* p = m_pInner->GetDataInfo(nID);
	if (NULL == p)
	{
		return nullptr;
	}

	DataInfo^ di = gcnew DataInfo();
	di->Inner = p;

	return di;
}
