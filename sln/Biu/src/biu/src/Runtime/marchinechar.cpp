
#include "stdafx.h"
#include "bmc.h"
#include "bmarco.h"
#include "bwmi.h"


using namespace Biu;


#define DISK_INFO_MODEL_LEN			40
#define DISK_INFO_SERIAL_LEN		20

#define DISK_INFO_MODEL_INDEX		54
#define DISK_INFO_SERIAL_INDEX		20


// Forwarding

VOID FilterStringA(CStringA& strID);
VOID FilterIDPart(CString& strIDPart);
BOOL GetDiskInfoRaw(LPSTR lpszModel, LPSTR lpszSerial);
BOOL GetBIOSInfoRaw(IWbemServices* pSvc, LPWSTR lpszManufacturer, DWORD cbManufacturer, LPWSTR lpszSerialNumber, DWORD cbSerialNumber);
BOOL GetCPUInfoRaw(IWbemServices* pSvc, LPWSTR lpszCPUID, DWORD cbCPUID);
BOOL GetPhysicalMemoryInfoRaw(IWbemServices* pSvc, LPWSTR lpszManufacturer, DWORD cbManufacturer, LPWSTR lpszSerialNumber, 
	DWORD cbSerialNumber, LPWSTR lpszPartNumber, DWORD cbPartNumber);


VOID
FilterStringA(CStringA& strID)
{
	static const CHAR Filter1 = '-';
	static const CHAR Filter2 = ' ';

	CHAR ch;
	int nIdx = 0;
	for (; nIdx < strID.GetLength(); nIdx++)
	{
		ch = strID[nIdx];
		if (!IsCharAlphaNumericA(ch))
		{
			if (Filter1 != ch && Filter2 != ch)
			{
				break;
			}
		}
	}

	strID = strID.Left(nIdx + 1);
}

VOID
FilterIDPart(CString& strIDPart)
{
	strIDPart.Remove(BMC_UNIQUEID_SEPARATOR);
	strIDPart.Remove(BMC_TOKEN_OMIT1);
}


BOOL
Biu::
CheckWMIService()
{
	return WMIIsServiceRunning();
}

BOOL
Biu::
InitializeWMI(BOOL bFullInit)
{
	return WMIInitializeContext(bFullInit);
}

VOID
Biu::
UninitializeWMI()
{
	WMIUninitializeContext();
}

BOOL
GetDiskInfoRaw(LPSTR lpszModel, LPSTR lpszSerial)
{
	ATLASSERT(lpszModel);
	ATLASSERT(lpszSerial);

	BOOL bRet = FALSE;
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	PSENDCMDOUTPARAMS pscop = NULL;

	TCHAR szSysDir[MAX_PATH] = { 0 };
	GetSystemDirectory(szSysDir, MAX_PATH);
	TCHAR szTargetDrive[7];
	_stprintf_s(szTargetDrive, 7, _T("\\\\.\\%c%c"), szSysDir[0], szSysDir[1]);

	__try
	{
		hDevice = CreateFile(szTargetDrive, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
		if (!hDevice)
		{
			__leave;
		}

		DWORD dwBytesReturned;

		VOLUME_DISK_EXTENTS vdExtents;
		ZeroMemory(&vdExtents, sizeof(vdExtents));
		
		if(!DeviceIoControl(hDevice, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0,
			&vdExtents, sizeof(vdExtents), &dwBytesReturned, NULL))
		{
			__leave;
		}

		GETVERSIONINPARAMS gvopVersionParams;
		ZeroMemory(&gvopVersionParams, sizeof(gvopVersionParams));
		
		if(!DeviceIoControl(hDevice, SMART_GET_VERSION, NULL, 0,
			&gvopVersionParams, sizeof(gvopVersionParams), &dwBytesReturned, NULL))
		{
			__leave;
		}

		SENDCMDINPARAMS scip = {
			IDENTIFY_BUFFER_SIZE,
			{
				0, 1, 1, 0, 0,
				((vdExtents.Extents[0].DiskNumber & 1) ? 0xB0 : 0xA0),
				((gvopVersionParams.bIDEDeviceMap >> vdExtents.Extents[0].DiskNumber & 0x10) ? ATAPI_ID_CMD : ID_CMD)
			},
			(BYTE)vdExtents.Extents[0].DiskNumber
			};

		DWORD dwOutBufSize = sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE;
		pscop = (PSENDCMDOUTPARAMS)new BYTE[dwOutBufSize];
		ZeroMemory(pscop, dwOutBufSize);

		if (!DeviceIoControl(hDevice, SMART_RCV_DRIVE_DATA,
			&scip, sizeof(SENDCMDINPARAMS) - 1, pscop, dwOutBufSize,
			&dwBytesReturned, NULL))
		{
			__leave;
		}

		USHORT *pIDSector = (USHORT*)pscop->bBuffer;
		for(int nShort = 10; nShort < 93 / 2; nShort++)
		{
			pIDSector[nShort] = (((pIDSector[nShort] & 0x00FF) << 8) + ((pIDSector[nShort] & 0xFF00) >> 8));
		}

		CopyMemory(lpszModel, pscop->bBuffer + DISK_INFO_MODEL_INDEX, DISK_INFO_MODEL_LEN);
		CopyMemory(lpszSerial, pscop->bBuffer + DISK_INFO_SERIAL_INDEX, DISK_INFO_SERIAL_LEN);

		bRet = TRUE;
	}
	__finally
	{
		if (pscop)
		{
			delete[] pscop;
			pscop = NULL;
		}

		if (INVALID_HANDLE_VALUE != hDevice)
		{
			CloseHandle(hDevice);
			hDevice = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}

BOOL
Biu::
GetDiskInfo(LPTSTR lpszModel, DWORD cbModel, LPTSTR lpszSerial, DWORD cbSerial)
{
	ATLASSERT(lpszModel);
	ATLASSERT(lpszSerial);

	CStringA	strModel,
				strSerial;

	BOOL bSucc = FALSE;

	bSucc = GetDiskInfoRaw(strModel.GetBuffer(DISK_INFO_MODEL_LEN), strSerial.GetBuffer(DISK_INFO_SERIAL_LEN));
	strModel.ReleaseBuffer();
	strSerial.ReleaseBuffer();
	strModel.Trim();
	strSerial.Trim();
	
	if (!bSucc)
	{
		return bSucc;
	}

	FilterStringA(strModel);
	FilterStringA(strSerial);

	_tcscpy_s(lpszModel, cbModel, CA2T(strModel.Trim()));
	_tcscpy_s(lpszSerial, cbSerial, CA2T(strSerial.Trim()));

	return TRUE;
}

BOOL
GetBIOSInfoRaw(
	IWbemServices* pSvc,
	LPWSTR lpszManufacturer,
	DWORD cbManufacturer,
	LPWSTR lpszSerialNumber,
	DWORD cbSerialNumber
	)
{
	ATLASSERT(lpszManufacturer);
	ATLASSERT(lpszSerialNumber);

	BOOL bRet = FALSE;
	BOOL bSolo = !pSvc;
	IWbemLocator* pLoc = NULL;
	IWbemClassObject* apClassObject[1];
	ULONG ulObjectCount = 1;

	BSTR bstrPropManufacturer = NULL;
	BSTR bstrPropSerialNumber = NULL;
	VARIANT vManufacturer;
	VARIANT vSerialNumber;

	__try
	{
		if (bSolo)
		{
			LEAVE(WMIInitialize(&pLoc, &pSvc));
		}

		LEAVE(WMIQueryObject(_T("Select * from Win32_BIOS"), &ulObjectCount, pSvc, apClassObject));

		bstrPropManufacturer = SysAllocString(L"Manufacturer");
		bstrPropSerialNumber = SysAllocString(L"SerialNumber");

		LEAVE_HR(apClassObject[0]->Get(bstrPropManufacturer, 0, &vManufacturer, 0, 0));
		LEAVE_HR(apClassObject[0]->Get(bstrPropSerialNumber, 0, &vSerialNumber, 0, 0));

		wcscpy_s(lpszManufacturer, cbManufacturer, vManufacturer.bstrVal);
		wcscpy_s(lpszSerialNumber, cbSerialNumber, vSerialNumber.bstrVal);

		bRet = TRUE;
	}
	__finally
	{
		SAFE_SYSFREE_STRING(bstrPropManufacturer);
		SAFE_SYSFREE_STRING(bstrPropSerialNumber);
		
		VariantClear(&vManufacturer);
		VariantClear(&vSerialNumber);
		
		SAFE_RELEASE(apClassObject[0]);

		if (bSolo)
		{
			WMIUninitialize(&pLoc, &pSvc);
		}
	}

	return bRet;
}

BOOL
Biu::
GetBIOSInfo(
	LPTSTR lpszManufacturer,
	DWORD cbManufacturer,
	LPTSTR lpszSerialNumber,
	DWORD cbSerialNumber,
	IWbemServices* pSvc
	)
{
	ATLASSERT(lpszManufacturer);
	ATLASSERT(lpszSerialNumber);

	CStringW	strManufacturer,
				strSerialNumber;

	BOOL bRet = FALSE;

	bRet = GetBIOSInfoRaw(pSvc, strManufacturer.GetBuffer(MAX_PATH), MAX_PATH, strSerialNumber.GetBuffer(MAX_PATH), MAX_PATH);
	strManufacturer.ReleaseBuffer();
	strSerialNumber.ReleaseBuffer();

	strManufacturer.Trim();
	strSerialNumber.Trim();

	_tcscpy_s(lpszManufacturer, cbManufacturer, CW2T(strManufacturer));
	_tcscpy_s(lpszSerialNumber, cbSerialNumber, CW2T(strSerialNumber));

	return bRet;
}

BOOL
GetCPUInfoRaw(
	IWbemServices* pSvc,
	LPWSTR lpszCPUID,
	DWORD cbCPUID
	)
{
	ATLASSERT(lpszCPUID);

	BOOL bRet = FALSE;
	BOOL bSolo = !pSvc;
	IWbemLocator* pLoc = NULL;
	IWbemClassObject* apClassObject[1];
	ULONG ulObjectCount = 1;

	BSTR bstrPropProcessorID = NULL;
	VARIANT vProcessorID;

	__try
	{
		if (bSolo)
		{
			LEAVE(WMIInitialize(&pLoc, &pSvc));
		}

		LEAVE(WMIQueryObject(_T("Select * from Win32_Processor"), &ulObjectCount, pSvc, apClassObject));

		bstrPropProcessorID = SysAllocString(L"ProcessorID");

		LEAVE_HR(apClassObject[0]->Get(bstrPropProcessorID, 0, &vProcessorID, 0, 0));

		wcscpy_s(lpszCPUID, cbCPUID, vProcessorID.bstrVal);

		bRet = TRUE;
	}
	__finally
	{
		SAFE_SYSFREE_STRING(bstrPropProcessorID);
		
		VariantClear(&vProcessorID);
		
		SAFE_RELEASE(apClassObject[0]);

		if (bSolo)
		{
			WMIUninitialize(&pLoc, &pSvc);
		}
	}

	return bRet;
}

BOOL
Biu::
GetCPUInfo(
	LPTSTR lpszCPUID,
	DWORD cbCPUID,
	IWbemServices* pSvc
	)
{
	ATLASSERT(lpszCPUID);

	CStringW strCPUID;

	BOOL bRet = FALSE;

	bRet = GetCPUInfoRaw(pSvc, strCPUID.GetBuffer(MAX_PATH), MAX_PATH);
	strCPUID.ReleaseBuffer();

	strCPUID.Trim();

	_tcscpy_s(lpszCPUID, cbCPUID, CW2T(strCPUID));

	return bRet;
}

BOOL
GetPhysicalMemoryInfoRaw(
	IWbemServices* pSvc,
	LPWSTR lpszManufacturer,
	DWORD cbManufacturer,
	LPWSTR lpszSerialNumber,
	DWORD cbSerialNumber,
	LPWSTR lpszPartNumber,
	DWORD cbPartNumber
	)
{
	ATLASSERT(lpszManufacturer);
	ATLASSERT(lpszSerialNumber);
	ATLASSERT(lpszPartNumber);

	BOOL bRet = FALSE;
	BOOL bSolo = !pSvc;
	IWbemLocator* pLoc = NULL;
	IWbemClassObject* apClassObject[1];
	ULONG ulObjectCount = 1;

	BSTR bstrPropManufacturer = NULL;
	BSTR bstrPropSerialNumber = NULL;
	BSTR bstrPropPartNumber = NULL;
	VARIANT vManufacturer;
	VARIANT vSerialNumber;
	VARIANT vPartNumber;

	__try
	{
		if (bSolo)
		{
			LEAVE(WMIInitialize(&pLoc, &pSvc));
		}

		LEAVE(WMIQueryObject(_T("Select * from Win32_PhysicalMemory"), &ulObjectCount, pSvc, apClassObject));

		bstrPropManufacturer = SysAllocString(L"Manufacturer");
		bstrPropSerialNumber = SysAllocString(L"SerialNumber");
		bstrPropPartNumber = SysAllocString(L"PartNumber");

		LEAVE_HR(apClassObject[0]->Get(bstrPropManufacturer, 0, &vManufacturer, 0, 0));
		LEAVE_HR(apClassObject[0]->Get(bstrPropSerialNumber, 0, &vSerialNumber, 0, 0));
		LEAVE_HR(apClassObject[0]->Get(bstrPropPartNumber, 0, &vPartNumber, 0, 0));

		wcscpy_s(lpszManufacturer, cbManufacturer, vManufacturer.bstrVal);
		wcscpy_s(lpszSerialNumber, cbSerialNumber, vSerialNumber.bstrVal);
		wcscpy_s(lpszPartNumber, cbPartNumber, vPartNumber.bstrVal);

		bRet = TRUE;
	}
	__finally
	{
		SAFE_SYSFREE_STRING(bstrPropManufacturer);
		SAFE_SYSFREE_STRING(bstrPropSerialNumber);
		SAFE_SYSFREE_STRING(bstrPropPartNumber);
		
		VariantClear(&vManufacturer);
		VariantClear(&vSerialNumber);
		VariantClear(&vPartNumber);
		
		SAFE_RELEASE(apClassObject[0]);

		if (bSolo)
		{
			WMIUninitialize(&pLoc, &pSvc);
		}
	}

	return bRet;
}

BOOL
Biu::
GetPhysicalMemoryInfo(
	LPTSTR lpszManufacturer,
	DWORD cbManufacturer,
	LPTSTR lpszSerialNumber,
	DWORD cbSerialNumber,
	LPTSTR lpszPartNumber,
	DWORD cbPartNumber,
	IWbemServices* pSvc
	)
{
	ATLASSERT(lpszManufacturer);
	ATLASSERT(lpszSerialNumber);
	ATLASSERT(lpszPartNumber);

	CStringW	strManufacturer,
				strSerialNumber,
				strPartNumber;

	BOOL bRet = FALSE;

	bRet = GetPhysicalMemoryInfoRaw(
		pSvc,
		strManufacturer.GetBuffer(MAX_PATH), MAX_PATH,
		strSerialNumber.GetBuffer(MAX_PATH), MAX_PATH,
		strPartNumber.GetBuffer(MAX_PATH), MAX_PATH
		);

	strManufacturer.ReleaseBuffer();
	strSerialNumber.ReleaseBuffer();
	strPartNumber.ReleaseBuffer();

	strManufacturer.Trim();
	strSerialNumber.Trim();
	strPartNumber.Trim();

	_tcscpy_s(lpszManufacturer, cbManufacturer, CW2T(strManufacturer));
	_tcscpy_s(lpszSerialNumber, cbSerialNumber, CW2T(strSerialNumber));
	_tcscpy_s(lpszPartNumber, cbPartNumber, CW2T(strPartNumber));

	return bRet;
}

BOOL
Biu::
GetNetworkAdapterInfo(
	LPTSTR lpszMac,
	DWORD cbMac
	)
{
	ATLASSERT(lpszMac);

	BOOL bRet = FALSE;
	CString strMac;
	ULONG cbBuf = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(cbBuf);
	
	if (ERROR_BUFFER_OVERFLOW == GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &cbBuf))
	{
		free(pAddresses);
		pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(cbBuf);
	}

	CHK_FALSE(pAddresses);

	DWORD dwRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &cbBuf);
	if (NO_ERROR == dwRetVal)
	{
		PIP_ADAPTER_ADDRESSES pcur = pAddresses;
		while(pcur)
		{
			if (pcur->PhysicalAddressLength)
			{
				CString strMacPart;

				//strMac.Append(CW2T(pcur->FriendlyName));
				//strMac.Append(_T(" : "));
				for (int i = 0; i < (int)pcur->PhysicalAddressLength; i++)
				{
					strMacPart.Format(_T("%.2X"), pcur->PhysicalAddress[i]);
					strMac.Append(strMacPart);
					if (i != (int)pcur->PhysicalAddressLength - 1)
					{
						strMac.Append(_T("-"));
					}
				}

				bRet = TRUE;
				break;
			}

			pcur = pcur->Next;
		}
	}
	else
	{
		if (ERROR_NO_DATA == dwRetVal)
		{
			bRet = FALSE;
		}
	}

	if (pAddresses)
	{
		free(pAddresses);
		pAddresses = NULL;
	}

	if (bRet)
	{
		_tcscpy_s(lpszMac, cbMac, strMac);
	}

	return bRet;
}

BOOL
Biu::
CreateMachineUniqueID(
	LPDWORD aIDType,
	DWORD cbIDType,
	LPTSTR lpszUniqueID,
	DWORD cbUniqueID
	)
{
	ATLASSERT(aIDType);
	ATLASSERT(cbIDType);
	ATLASSERT(lpszUniqueID);

	CHK_FALSE(CheckWMIService());

	IWbemServices* pSvc = NULL;
	IWbemLocator* pLoc = NULL;

	CHK_FALSE(WMIInitialize(&pLoc, &pSvc));

	BOOL bRet = FALSE;
	CString strUniqieID;
	for (DWORD i = 0; i < cbIDType; i++)
	{
		switch (aIDType[i])
		{
		case BMC_UNIQUEID_DISK:
			{
				CString strModel,
						strSerialNumber;

				bRet = GetDiskInfo(
					strModel.GetBuffer(DISK_INFO_MODEL_LEN), DISK_INFO_MODEL_LEN,
					strSerialNumber.GetBuffer(DISK_INFO_SERIAL_LEN), DISK_INFO_SERIAL_LEN
					);

				strModel.ReleaseBuffer();
				strSerialNumber.ReleaseBuffer();

				if (bRet)
				{
					FilterIDPart(strModel);
					FilterIDPart(strSerialNumber);
					strUniqieID += strModel + BMC_UNIQUEID_SEPARATOR + strSerialNumber;
				}
			}
			break;
		case BMC_UNIQUEID_BIOS:
			{
				CString strManufacturer,
						strSerialNumber;

				bRet = GetBIOSInfo(
					strManufacturer.GetBuffer(MAX_PATH), MAX_PATH,
					strSerialNumber.GetBuffer(MAX_PATH), MAX_PATH,
					pSvc
					);

				strManufacturer.ReleaseBuffer();
				strSerialNumber.ReleaseBuffer();

				if (bRet)
				{
					FilterIDPart(strManufacturer);
					FilterIDPart(strSerialNumber);
					strUniqieID += strManufacturer + BMC_UNIQUEID_SEPARATOR + strSerialNumber;
				}
			}
			break;
		case BMC_UNIQUEID_MEM:
			{
				CString strManufacturer,
						strSerialNumber,
						strPartNumber;

				bRet = GetPhysicalMemoryInfo(
					strManufacturer.GetBuffer(MAX_PATH), MAX_PATH,
					strSerialNumber.GetBuffer(MAX_PATH), MAX_PATH,
					strPartNumber.GetBuffer(MAX_PATH), MAX_PATH,
					pSvc
					);

				strManufacturer.ReleaseBuffer();
				strSerialNumber.ReleaseBuffer();
				strPartNumber.ReleaseBuffer();

				if (bRet)
				{
					FilterIDPart(strManufacturer);
					FilterIDPart(strSerialNumber);
					FilterIDPart(strPartNumber);
					strUniqieID += strManufacturer + BMC_UNIQUEID_SEPARATOR + strSerialNumber + BMC_UNIQUEID_SEPARATOR + strPartNumber;
				}
			}
			break;
		case BMC_UNIQUEID_NET:
			{
				CString strMac;

				bRet = GetNetworkAdapterInfo(
					strMac.GetBuffer(MAX_PATH), MAX_PATH
					);

				strMac.ReleaseBuffer();

				if (bRet)
				{
					strUniqieID += strMac;
				}
			}
			break;
		case BMC_UNIQUEID_CPU:
			{
				CString strCPUID;

				bRet = GetCPUInfo(
					strCPUID.GetBuffer(MAX_PATH), MAX_PATH,
					pSvc
					);

				strCPUID.ReleaseBuffer();

				if (bRet)
				{
					FilterIDPart(strCPUID);
					strUniqieID += strCPUID;
				}
			}
			break;
		}

		if (!bRet)
		{
			break;
		}
	}

	if (bRet)
	{
		_tcscpy_s(lpszUniqueID, cbUniqueID, strUniqieID);
	}

	WMIUninitialize(&pLoc, &pSvc);

	return bRet;
}

BOOL 
Biu::
CreateMachineDefaultUniqueID(
	LPTSTR lpszUniqueID,
	DWORD cbUniqueID
	)
{
	static DWORD aIDType[] = 
	{
		BMC_UNIQUEID_DISK,
		BMC_UNIQUEID_BIOS,
		BMC_UNIQUEID_MEM,
		BMC_UNIQUEID_NET,
		BMC_UNIQUEID_CPU
	};

	static DWORD cbIDType = sizeof(aIDType) / sizeof(aIDType[0]);

	return Biu::CreateMachineUniqueID(aIDType, cbIDType, lpszUniqueID, cbUniqueID);
}

BOOL
Biu::
CreateMachineDefaultUniqueIDForUser(
	LPTSTR lpszUniqueID,
	DWORD cbUniqueID
	)
{
	static DWORD aIDType[] =
	{
		BMC_UNIQUEID_BIOS,
		BMC_UNIQUEID_MEM,
		BMC_UNIQUEID_NET,
		BMC_UNIQUEID_CPU
	};

	static DWORD cbIDType = sizeof(aIDType) / sizeof(aIDType[0]);

	return Biu::CreateMachineUniqueID(aIDType, cbIDType, lpszUniqueID, cbUniqueID);
}
