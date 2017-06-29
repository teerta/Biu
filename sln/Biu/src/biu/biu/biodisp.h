
#ifndef _BIODISP_H_
#define _BIODISP_H_


#include "biu.h"
#include "bbase.h"
#include "bptl.h"
#include "biodef.h"
#include "berr.h"


namespace Biu {


	// Forwarding

	class BIODispatcher;
	class BIOHandler;
	struct BIOContext;
	class BIOExecutionUnit;
	class BSecurityProvider;
	class BIOSecurityContext;
	class BIOBehaviorArbiter;



	// Class BIODispatchable

	class BIU_API BIODispatchable
	{
	public:
		BIODispatchable();
		virtual ~BIODispatchable();

#ifdef _WINDOWS
		virtual HANDLE GetCircleHandle() = 0;
#else
		virtual int GetCircleHandle() = 0;
#endif

		virtual void HandleError(BCallingSite callingSite, ssize_t err) = 0;

		virtual void Clear();


		void SetTimeout(long long llSeconds);


		BIOExecutionUnit* GetExecutionUnit();
		void SetExecutionUnit(BIOExecutionUnit* pUnit);

		IBDataChannel* GetProtocol();
		void SetProtocol(IBDataChannel* pProtocol);

		BIOHandler* GetHandler();
		void SetHandler(BIOHandler* pHandler);

		bool IsInTheCircle();
		void SetInTheCircle(bool val = true);

#ifndef _WINDOWS
		bool IsInTheCircleForWriting();
		void SetInTheCircleForWriting(bool val = true);
#endif

		bool IsWriting();
		void SetWriting(bool val = true);

		unsigned int GetRefIO();
		void IncRefIO();
		void DecRefIO();

		long long GetVar();
		void SetVar(long long llVar);

		B_IO_Read_Mode GetMode();
		void SetMode(B_IO_Read_Mode mode);

		long long GetTimeout();

		void SetPending(bool b = true);
		bool IsPending();

		void ConfigSecurity(BSecurityProvider* pProv);
		BIOSecurityContext* GetSecurityContext();

		void ConfigBehaviorArbiter();
		BIOBehaviorArbiter* GetBehaviorArbiter();


		inline BHANDLE GetIntern() { return m_pIntern; }

	protected:
		BHANDLE m_pIntern;
	};


	// Class BIOExecutionUnit

	class BIU_API BIOExecutionUnit
	{
	public:
		BIOExecutionUnit();
		BIOExecutionUnit(const BIOExecutionUnit& rhs) = delete;
		virtual ~BIOExecutionUnit();
		
		BIOExecutionUnit& operator=(const BIOExecutionUnit& rhs) = delete;

		//virtual void ExecuteIO(LPOVERLAPPED_ENTRY poe, BOOL bSucc) = 0;
		virtual void ExecuteIO(DWORD cb, ULONG_PTR lpCompletionKey, LPOVERLAPPED lpOverlapped, BOOL bSucc) = 0;
#ifdef _WINDOWS
		virtual void ExecuteWorker(BIOContext* pContext) = 0;
#else
		virtual bool ExecuteWorker(BIOContext* pContext, shared_ptr<list<BIOContext*> >& listPending) = 0;
#endif

		virtual void Reclaim(BIODispatchable* pDisp);

		inline BIODispatcher* GetDispatcher() { return m_pDispatcher; }
		inline void SetDispatcher(BIODispatcher* pDispatcher) { m_pDispatcher = pDispatcher; }

	protected:
		BIODispatcher* m_pDispatcher;
	};


	// Class BIODispatcher

	class BIU_API BIODispatcher
	{
	public:
		BIODispatcher();
		BIODispatcher(const BIODispatcher& rhs) = delete;		
		virtual ~BIODispatcher();

		BIODispatcher& operator=(const BIODispatcher& rhs) = delete;

		bool Create(BMemoryShack_char* pShack, unsigned int ulIOs = 0, unsigned int ulWorkers = 0);
		void Close();

		void PushWork(BIOContext* pContext);
		BIOContext* PopWork();

		bool JoinTheCircle(BIODispatchable* pDisp);
#ifndef _WINDOWS
		bool JoinTheCircleForWriting(BIODispatchable* pDisp);
#endif

		void AddUnit(BIOExecutionUnit* pUnit);

		BMemoryShack_char* GetShack();


		inline BHANDLE GetIntern() { return m_pIntern; }

	private:
		BHANDLE m_pIntern;
	};


}


#endif // _BIODISP_H_
