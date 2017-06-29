
#pragma once


#include "biu.h"
#include <wbemidl.h>


#ifdef __cplusplus
extern "C"{
#endif


namespace Biu {


// Check WMI Service
BIU_API
BOOL
WMIVerifyService();

BIU_API
BOOL
WMIIsServiceRunning();

// Initialize COM & COM Security (Default Security)
BIU_API
BOOL
WMIInitializeContext(BOOL bFullInit = TRUE);

// Uninitialize COM
BIU_API
VOID
WMIUninitializeContext();

BIU_API
BOOL
WMIInitialize(IWbemLocator** ppLoc, IWbemServices** ppSvc);

BIU_API
VOID
WMIUninitialize(IWbemLocator** ppLoc, IWbemServices** ppSvc);

BIU_API
BOOL
WMIExecuteQuery(LPCTSTR lpszQuery, IWbemServices* pSvc, IEnumWbemClassObject** ppEnumObject);

BIU_API
BOOL
WMIQueryObject(LPCTSTR lpszQuery, PULONG pulObjectCount, IWbemServices* pSvc, IWbemClassObject** apClassObject);


}


#ifdef __cplusplus
}
#endif
