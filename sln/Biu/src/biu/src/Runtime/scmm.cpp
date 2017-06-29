
#include "stdafx.h"
#include "bscm.h"


using namespace Biu;


#define SCMM_ARG_INS_SERVICE		_T("-Service")



BSCMManager::BSCMManager()
: m_hSCM(NULL)
, m_hService(NULL)
, m_strServiceName("")
{
}

BSCMManager::~BSCMManager()
{
	ServiceClose();
	CloseSCM();
}

bool BSCMManager::OpenSCM(DWORD rights)
{
	m_dwRights = rights;
	m_hSCM = OpenSCManager(NULL, NULL, rights);

	return m_hSCM != NULL;
}

void BSCMManager::CloseSCM()
{
	if (m_hSCM)
	{
		CloseServiceHandle(m_hSCM);
		m_hSCM = NULL;
	}
}

bool BSCMManager::ServiceStop()
{
	if (!m_hSCM || !m_hService)
	{
		return false;
	}

	SERVICE_STATUS_PROCESS ssStatus;
	ULONGLONG ullStartTime = GetTickCount64();
	DWORD dwBytesNeeded;
	DWORD dwTimeout = 30000;

	if (!QueryServiceStatusEx(m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ))
	{
		return false;
	}

	if (ssStatus.dwCurrentState == SERVICE_STOPPED)
	{
		return true;
	}

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		Sleep(ssStatus.dwWaitHint);

		if (!QueryServiceStatusEx(
			m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
		{
			return false;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOPPED)
		{
			return true;
		}

		if (GetTickCount64() - ullStartTime > dwTimeout)
		{
			OutputDebugString(_T("[XSCMM] Service stop timed out\r\n"));
			return false;
		}
	}

	StopDependentServices();

	if (!ControlService(m_hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssStatus))
	{
		return false;
	}

	while (ssStatus.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssStatus.dwWaitHint);

		if (!QueryServiceStatusEx(
			m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
		{
			return false;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOPPED)
		{
			break;
		}

		if (GetTickCount64() - ullStartTime > dwTimeout)
		{
			OutputDebugString(_T("[XSCMM] Wait timed out\r\n"));
			return false;
		}
	}

	return true;
}

bool BSCMManager::ServiceStart()
{
	if (!m_hSCM || !m_hService)
	{
		return false;
	}

	SERVICE_STATUS_PROCESS ssStatus; 
	DWORD dwOldCheckPoint; 
	ULONGLONG ullStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	if (!QueryServiceStatusEx(m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ))
	{
		return false;
	}

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		return true;
	}

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		ullStartTickCount = GetTickCount64();
		dwOldCheckPoint = ssStatus.dwCheckPoint;

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
		{
			dwWaitTime = 1000;
		}
		else if (dwWaitTime > 10000)
		{
			dwWaitTime = 10000;
		}

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
		{
			return false;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			ullStartTickCount = GetTickCount64();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if(GetTickCount64() - ullStartTickCount > ssStatus.dwWaitHint)
			{
				OutputDebugString(_T("[XSCMM] Timeout waiting for service to stop\r\n"));
				return false;
			}
		}
	}

	if (!StartService(m_hService, 0, NULL))
	{
		OutputDebugString(_T("[XSCMM] StartService failed\r\n"));
		return false;
	}

	if (!QueryServiceStatusEx(m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
	{
		return false;
	}

	ullStartTickCount = GetTickCount64();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
	{ 
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
		{
			dwWaitTime = 1000;
		}
		else if (dwWaitTime > 10000)
		{
			dwWaitTime = 10000;
		}

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssStatus, sizeof(SERVICE_STATUS_PROCESS),&dwBytesNeeded))
		{
			break;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			ullStartTickCount = GetTickCount64();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if(GetTickCount64() - ullStartTickCount > ssStatus.dwWaitHint)
			{
				break;
			}
		}
	}

	return ssStatus.dwCurrentState == SERVICE_RUNNING;
}

bool BSCMManager::ServiceOpen(LPCTSTR lpszServiceName, DWORD rights)
{
	if (!m_hSCM)
	{
		return false;
	}

	ServiceClose();

	m_strServiceName = lpszServiceName;
	m_dwServiceRights = rights;
	m_hService = OpenService(m_hSCM, lpszServiceName, rights);

	return m_hService != NULL;
}

void BSCMManager::ServiceClose()
{
	if (m_hService)
	{
		CloseServiceHandle(m_hService);
		m_hService = NULL;
	}
}

bool BSCMManager::StopDependentServices()
{
	DWORD i;
	DWORD dwBytesNeeded;
	DWORD dwCount;

	LPENUM_SERVICE_STATUS   lpDependencies = NULL;
	ENUM_SERVICE_STATUS     ess;
	SC_HANDLE               hDepService;
	SERVICE_STATUS_PROCESS  ssp;

	ULONGLONG ullStartTime = GetTickCount64();
	DWORD dwTimeout = 30000;


	if (EnumDependentServices(m_hService, SERVICE_ACTIVE, lpDependencies, 0, &dwBytesNeeded, &dwCount))
	{
		 // If the Enum call succeeds, then there are no dependent services, so do nothing.
		 return true;
	} 
	else 
	{
		if (GetLastError() != ERROR_MORE_DATA)
		{
			return false; // Unexpected error
		}

		lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

		if (!lpDependencies)
		{
			return false;
		}

		__try
		{
			if (!EnumDependentServices(m_hService, SERVICE_ACTIVE, lpDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount))
			{
				return false;
			}

			for (i = 0; i < dwCount; i++)
			{
				ess = *(lpDependencies + i);
				
				hDepService = OpenService(m_hSCM, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);

				if (!hDepService)
				{
				   return false;
				}

				__try
				{
					if (!ControlService(hDepService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp))
					{
						return false;
					}

					while (ssp.dwCurrentState != SERVICE_STOPPED)
					{
						Sleep(ssp.dwWaitHint);

						if (!QueryServiceStatusEx(hDepService, SC_STATUS_PROCESS_INFO,
							(LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
						{
							return false;
						}

						if (ssp.dwCurrentState == SERVICE_STOPPED)
						{
							break;
						}

						if (GetTickCount64() - ullStartTime > dwTimeout)
						{
							return false;
						}
					}
				} 
				__finally 
				{
					CloseServiceHandle(hDepService);
				}
			}
		} 
		__finally 
		{
			HeapFree(GetProcessHeap(), 0, lpDependencies);
		}
	} 

	return true;
}

void BSCMManager::ServiceInstallSelf(LPCTSTR lpszBinPath)
{
	ShellExecute(NULL, _T("open"), lpszBinPath, SCMM_ARG_INS_SERVICE, NULL, SW_HIDE);
}

bool BSCMManager::ServiceTryOpen(LPCTSTR lpszServiceName, LPCTSTR lpszBinPath)
{
	static const int Total_Solution = 1;

	bool bRet = false;
	int nCounter = 0;

	bRet = ServiceOpen(lpszServiceName);
	while (!bRet)
	{
		switch (nCounter)
		{
		case 0:
			ServiceInstallSelf(lpszBinPath);
			break;
		//case 1:
		//	break;
		default:
			bRet = true;
			break;
		}

		nCounter++;

		if (!bRet)
		{
			Sleep(1000);
			bRet = ServiceOpen(lpszServiceName);
		}
	}

	if (nCounter > Total_Solution)
	{
		bRet = false;
	}

	return bRet;
}

bool BSCMManager::IsServiceRunning()
{
	if (!m_hSCM || !m_hService)
	{
		return false;
	}

	SERVICE_STATUS_PROCESS ssStatus;
	//DWORD dwOldCheckPoint;
	//DWORD dwStartTickCount;
	//DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	if (!QueryServiceStatusEx(m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded))
	{
		return false;
	}

	return ssStatus.dwCurrentState == SERVICE_RUNNING;
}
