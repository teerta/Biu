
#ifndef _I_IODISPAB_H_
#define _I_IODISPAB_H_


#include "biohand.h"
#include <atomic>


namespace Biu {


	// Forwoarding

	class BIODispatchable;
	struct IBDataChannel;
	class BIOExecutionUnit;
	class BIOHandler;
	struct BIOContext;
	class BIOSecurityContext;
	class BSecurityProvider;
	class BIOBehaviorArbiter;


	// Struct I_IODispatchable

	struct I_IODispatchable
	{
		I_IODispatchable();
		virtual ~I_IODispatchable();


		BIODispatchable* m_pOutter;
		
		BIOExecutionUnit* m_pExecUnit;
		IBDataChannel* m_pProtocol;
		BIOHandler* m_pHandler;
		BIOContext* m_pReadContext;
		BIOContext* m_pWriteContext;

		atomic_bool m_bInTheCircle;
#ifndef _WINDOWS
		atomic_bool m_bInTheCircleWrite;
#endif
		atomic_bool m_bReading;
		atomic_bool m_bWriting;
		atomic_uint m_refIO;
		atomic_uint m_refReadIO;
		atomic_bool m_bPending;

		time_t m_tBeginning;
		long long m_llTimeout;
		long long m_llVar;

		B_IO_Read_Mode m_mode;

		BIOSecurityContext* m_pSecurityContext;
		BIOBehaviorArbiter* m_pBehaviorArbiter;


		void Clear();

		void ConfigSecurity(BSecurityProvider* pProv);
		
		virtual void ConfigBehaviorArbiter();
		virtual BIOBehaviorArbiter* GetBehaviorArbiter();


		inline bool NotifyNewHandle()
		{			
			return m_pHandler->HandleCommand(m_pOutter, BIO_CMD_NEW_HANDLE, NULL, NULL);
		}

		inline void NotifyError(BCallingSite callingSite, ssize_t err)
		{			
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_ERROR, (void*)callingSite, (void*)err);
		}

		inline void NotifyTimeout()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_TIMEOUT, NULL, NULL);
		}

		inline void NotifyWriteDone(ssize_t cb)
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_WRITE_DONE, (void*)cb, NULL);
		}

		inline void NotifyClose()
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_CLOSE, NULL, NULL);
		}

		inline void NotifyCloseEx(bool bProactive)
		{
			m_pHandler->HandleCommand(m_pOutter, bProactive ? BIO_CMD_CLOSE_PROACTIVE : BIO_CMD_CLOSE_PASSIVE, NULL, NULL);
		}

		inline void NotifyFile(int id, void* p)
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_FILE, (void*)id, p);
		}

		inline void NotifyFileNext(void* p1, void* p2)
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_FILE_NEXT, p1, p2);
		}

		inline void NotifyDataHeader(void* p)
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_DATA_HEADER, p, NULL);
		}

		inline void NotifyBadCraft(B_IO_Bad_Craft_Type type, void* p)
		{
			m_pHandler->HandleCommand(m_pOutter, BIO_CMD_BAD_CRAFT, (void*)type, p);
		}

	};



}



#endif // _I_IODISPAB_H_
