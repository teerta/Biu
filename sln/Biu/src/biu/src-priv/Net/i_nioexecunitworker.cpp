
#include "stdafx.h"
#include "priv/i_nio.h"
#include "priv/i_peer.h"
#include "priv/bioctxt.h"


using namespace Biu;


#ifdef _WINDOWS
void I_NetIOExecutionUnit::ExecuteWorker(BIOContext* pContext)
{
	BPeer* peer = static_cast<BPeer*>(pContext->m_pDisp);
	I_Peer* pIntern = reinterpret_cast<I_Peer*>(peer->GetIntern());
	peer->DecRefIO();

	DWORD op = pContext->m_operation;
	if (BIO_Read & op)
	{
		pIntern->Craft();
	}
}
#endif

#ifndef _WINDOWS
bool I_NetIOExecutionUnit::ExecuteWorker(BIOContext* pContext, shared_ptr<list<BIOContext*> >& listPending)
{
	BPeer* peer = static_cast<BPeer*>(pContext->m_pDisp);
	peer->DecRefIO();

	ssize_t nRet = 0;
	if (BIO_Read == pContext->m_operation)
	{
		// Read (a reading has done)

		peer->Craft();
	}
	else if (BIO_Write == pContext->m_operation)
	{
		// Write (writing now)

		if (peer->m_bWriting && BPCF_Close != peer->m_closeFlag &&
			listPending->end() == find(listPending->begin(), listPending->end(), pContext))
		{
			peer->IncRefIO();
			listPending->push_back(pContext);
		}
		else
		{
			nRet = peer->Write();
			if (nRet)
			{
				if (EAGAIN == nRet)
				{
					pContext = m_pDispatcher->PopWork();
					return false;
				}
				else
				{
					peer->HandleError(nRet);
					// close peer here
					peer->RemoveFromCollection();
					peer->Destroy();
				}
			}
		}
	}

	return true;
}
#endif
