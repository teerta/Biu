
#include "stdafx.h"
#include "bwmi.h"
#include "bscm.h"
#include "bmarco.h"


//#pragma comment(lib, "wbemuuid.lib")


using namespace Biu;


#define WMI_QUERY_LAN				L"WQL"

#define SERVICE_WMI					_T("Winmgmt")


BOOL
Biu::
WMIInitialize(IWbemLocator** ppLoc, IWbemServices** ppSvc)
{
	VERIFY_HR_FALSE(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)ppLoc));
	
	// Connect to the root\default namespace with the current user.
	//VERIFY_HR_FALSE((*ppLoc)->ConnectServer(BSTR(L"ROOT\\DEFAULT"), NULL, NULL, 0, NULL, 0, 0, ppSvc));
	VERIFY_HR_FALSE((*ppLoc)->ConnectServer(BSTR(L"root\\cimv2"), NULL, NULL, 0, NULL, 0, 0, ppSvc));

	// Set the proxy so that impersonation of the client occurs.
	VERIFY_HR_FALSE(CoSetProxyBlanket(
		*ppSvc,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,EOAC_NONE
		));

	return TRUE;
}

VOID
Biu::
WMIUninitialize(IWbemLocator** ppLoc, IWbemServices** ppSvc)
{
	if (*ppSvc)
	{
		(*ppSvc)->Release();
		(*ppSvc) = NULL;
	}

	if (*ppLoc)
	{
		(*ppLoc)->Release();
		(*ppLoc) = NULL;
	}
}

BOOL
Biu::
WMIExecuteQuery(LPCTSTR lpszQuery, IWbemServices* pSvc, IEnumWbemClassObject** ppEnumObject)
{
	CHK_FALSE(pSvc);
	CHK_FALSE(ppEnumObject);

	VERIFY_HR_FALSE(pSvc->ExecQuery(WMI_QUERY_LAN, CT2OLE(lpszQuery), WBEM_FLAG_RETURN_IMMEDIATELY, NULL, ppEnumObject));
	RETURN_HR((*ppEnumObject)->Reset());
}

BOOL
Biu::
WMIInitializeContext(BOOL bFullInit)
{
	if (bFullInit)
	{
		VERIFY_HR_FALSE(CoInitialize(NULL));
	}

	RETURN_HR(CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL))
}

VOID
Biu::
WMIUninitializeContext()
{
	CoUninitialize();
}

BOOL
Biu::
WMIVerifyService()
{
	BSCMManager scmm;
	CHK_FALSE(scmm.OpenSCM());
	CHK_FALSE(scmm.ServiceOpen(SERVICE_WMI));

	return scmm.ServiceStart();
}

BOOL
Biu::
WMIIsServiceRunning()
{
	BSCMManager scmm;
	CHK_FALSE(scmm.OpenSCMForUser());
	CHK_FALSE(scmm.ServiceOpenForUser(SERVICE_WMI));

	return scmm.IsServiceRunning();
}

BOOL
Biu::
WMIQueryObject(LPCTSTR lpszQuery, PULONG pulObjectCount, IWbemServices* pSvc, IWbemClassObject** apClassObject)
{
	BOOL bRet = FALSE;
	IEnumWbemClassObject* pEnumObject = NULL;

	__try
	{
		if (!(*pulObjectCount))
		{
			*pulObjectCount = 1;
		}

		LEAVE(WMIExecuteQuery(lpszQuery, pSvc, &pEnumObject));
		
		ULONG ulReturned;
		HRESULT hr = WBEM_S_NO_ERROR;
		while (WBEM_S_NO_ERROR == hr)
		{
			hr = pEnumObject->Next(WBEM_INFINITE, *pulObjectCount, apClassObject, &ulReturned);
			if (SUCCEEDED(hr) && ulReturned)
			{
				*pulObjectCount = ulReturned;
				bRet = TRUE;
			}

			break;
		}
	}
	__finally
	{
		SAFE_RELEASE(pEnumObject);
	}

	return bRet;
}
