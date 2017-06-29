
#include "stdafx.h"
#include "bpeer.h"
#include "priv/i_peer.h"
#include "priv/bioctxt.h"
#include "bbase.h"
#include "bstctl.h"
#include "bpeersvr.h"
#include "bnio.h"
#include "basobj.h"
#include <cassert>

#ifdef _WINDOWS
#include "priv/bnex.h"
#else
#include <unistd.h>
#include <fcntl.h>
#endif


using namespace Biu;


// BPeer

BPeer::BPeer()
	: BIODispatchable()
{	
	auto* pIntern = new I_Peer();
	pIntern->m_pOutter = this;

	m_pIntern = (BHANDLE)pIntern;

	pIntern->m_pReadContext->m_operation = BIO_Accept;
	pIntern->m_pWriteContext->m_operation = BIO_Write;
}

BPeer::~BPeer()
{
	CHK(m_pIntern);

	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	delete pIntern;
	m_pIntern = NULL;
}

#ifdef _WINDOWS
HANDLE BPeer::GetCircleHandle()
{	
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return (HANDLE)pIntern->m_s;
}
#else
int BPeer::GetCircleHandle()
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_s;
}
#endif

void BPeer::HandleError(BCallingSite callingSite, ssize_t err)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	pIntern->HandleError(callingSite, err);
}

void BPeer::Clear()
{
	BIODispatchable::Clear();

	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	pIntern->Clear();
}

B_Peer_Transmit_Result BPeer::Send(IBSerializable* pObj, void* p)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->Send(pObj, p);
}

B_Peer_Transmit_Result BPeer::SendPod(char* pPod, size_t cbPod)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->SendPod(pPod, cbPod);
}

#ifdef _WINDOWS
B_Peer_Transmit_Result BPeer::OutterSend(void* pObj, void* p)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->OutterSend(pObj, p);
}
#endif

bool BPeer::Return(IBSerializable* pObj, void* p)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->Return(pObj, p);
}

bool BPeer::ReturnPod(char* pPod, size_t cbPod)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ReturnPod(pPod, cbPod);
}

#ifdef _WINDOWS
bool BPeer::OutterReturn(void* pObj, void* p)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->OutterReturn(pObj, p);
}
#endif

BDataInfo* BPeer::GetDataInfo(int nID)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->GetDataInfo(nID);
}

B_Peer_Transmit_Result BPeer::InitSendFile(LPCTSTR lpszFile, void* pHeader, int& nID)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->InitSendFile(lpszFile, pHeader, nID);
}

B_Peer_Transmit_Result BPeer::InitRecvFile(int nID, unsigned long long cb, LPCTSTR lpszFile)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->InitRecvFile(nID, cb, lpszFile);
}

bool BPeer::ClientSendFile(int nID)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ClientSendFile(nID);
}

bool BPeer::ClientSendFileFinal(int nID)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ClientSendFileFinal(nID);
}

B_Peer_Transmit_Result BPeer::ClientRecvFile(int nID, IBSerializable* pObj, void* p)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ClientRecvFile(nID, pObj, p);
}

B_Peer_Transmit_Result BPeer::ClientRecvFileFinal(int nID)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ClientRecvFileFinal(nID);
}

bool BPeer::ClientRecvFileData(int nID, BMemoryBrick* pBrick, size_t nPos)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ClientRecvFileData(nID, pBrick, nPos);
}

bool BPeer::ServerRecvFile(int nID, BMemoryBrick* pBrick, size_t nPos)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ServerRecvFile(nID, pBrick, nPos);
}

bool BPeer::ServerSendFile(int nID)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->ServerSendFile(nID);
}

void BPeer::TerminateTransmitting(int nID)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->TerminateTransmitting(nID);
}

void BPeer::TerminateAllTransmittings(BPeerDataInfo** ppPDI)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->TerminateAllTransmittings(ppPDI);
}

#ifndef _WINDOWS
int BPeer::GetOptError()
{
	return pIntern->GetOptError();
}
#endif

void BPeer::SetSendFlags(int nFlags) 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	pIntern->m_nSendFlags = nFlags; 
}

void BPeer::SetRecvFlags(int nFlags) 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	pIntern->m_nRecvFlags = nFlags;
}

B_Sock_Type BPeer::GetPeerSockType() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_sockType;
}

BPeerRepresentation* BPeer::GetRepresentation() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return &pIntern->m_representation;
}

BPeerRepresentation* BPeer::GetRemoteRepresentation() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return &pIntern->m_remoteRepresentation;
}

bool BPeer::IsServer() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_bServer;
}

unsigned int BPeer::GetID() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_nID;
}

unsigned int BPeer::GetServerID() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_nServerID;
}

BServerPeer* BPeer::GetServer() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_pServer;
}

void BPeer::SetRefID(unsigned int nID) 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	pIntern->m_nRefID = nID;
}

unsigned int BPeer::GetRefID() 
{ 
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_nRefID;
}

B_Sock_Trait BPeer::GetTrait()
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_trait;
}

void* BPeer::GetTraitVal()
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->m_traitVal;
}

BMemoryBrick* BPeer::MoveOutBrick()
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->MoveOutBrick();
}

void BPeer::MoveInBrick(BMemoryBrick* pBrick)
{
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(m_pIntern);
	return pIntern->MoveInBrick(pBrick);
}
