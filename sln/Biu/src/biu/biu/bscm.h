
#pragma once


#include "biu.h"
#include <winsvc.h>


namespace Biu {


class BIU_API BSCMManager
{
public:
	BSCMManager();
	virtual ~BSCMManager();

	bool OpenSCM(DWORD rights = SC_MANAGER_ALL_ACCESS);
	void CloseSCM();

	bool ServiceOpen(LPCTSTR lpszServiceName, DWORD rights = SERVICE_ALL_ACCESS);
	void ServiceClose();
	bool ServiceStart();
	bool ServiceStop();
	void ServiceInstallSelf(LPCTSTR lpszBinPath);

	bool ServiceTryOpen(LPCTSTR lpszServiceName, LPCTSTR lpszBinPath);
	
	bool IsServiceRunning();


	inline bool OpenSCMForUser()
	{
		return OpenSCM(STANDARD_RIGHTS_READ | SC_MANAGER_CONNECT | SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_QUERY_LOCK_STATUS);
	}

	inline bool ServiceOpenForUser(LPCTSTR lpszServiceName)
	{
		return ServiceOpen(lpszServiceName, READ_CONTROL | SERVICE_ENUMERATE_DEPENDENTS | SERVICE_INTERROGATE | 
			SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_USER_DEFINED_CONTROL);
	}


private:
	SC_HANDLE m_hSCM;
	SC_HANDLE m_hService;
	CString m_strServiceName;
	DWORD m_dwRights;
	DWORD m_dwServiceRights;

	bool StopDependentServices();
};


}
