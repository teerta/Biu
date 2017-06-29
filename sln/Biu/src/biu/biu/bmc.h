
#ifndef _BMC_H_
#define _BMC_H_


#include "biu.h"


#ifdef __cplusplus
extern "C"{
#endif


namespace Biu {


	#include <wbemidl.h>


	#define BMC_UNIQUEID_DISK			0x00000001
	#define BMC_UNIQUEID_BIOS			0x00000002
	#define BMC_UNIQUEID_MEM			0x00000003
	#define BMC_UNIQUEID_NET			0x00000004
	#define BMC_UNIQUEID_CPU			0x00000005

	#define BMC_UNIQUEID_SEPARATOR		_T('-')
	#define BMC_TOKEN_OMIT1				_T(' ')



	// Wrapper for WMI Helper Function
	BIU_API
	BOOL
	CheckWMIService();

	// Wrapper for WMI Helper Function
	BIU_API
	BOOL
	InitializeWMI(BOOL bFullInit = TRUE);

	// Wrapper for WMI Helper Function
	BIU_API
	VOID
	UninitializeWMI();

	// Disk Information (Model & Serial Number)
	BIU_API
	BOOL
	GetDiskInfo(
		LPTSTR lpszModel,
		DWORD cbModel,
		LPTSTR lpszSerial,
		DWORD cbSerial
		);

	// BIOS Information (Manufacturer & Serial Number)
	BIU_API
	BOOL
	GetBIOSInfo(
		LPTSTR lpszManufacturer,
		DWORD cbManufacturer,
		LPTSTR lpszSerialNumber,
		DWORD cbSerialNumber,
		IWbemServices* pSvc = NULL
		);

	// CPU Information (CPUID)
	BIU_API
	BOOL
	GetCPUInfo(
		LPTSTR lpszCPUID,
		DWORD cbCPUID,
		IWbemServices* pSvc = NULL
		);

	// Physical Memory Information (Manufacturer, Serial Number & Part Number)
	BIU_API
	BOOL
	GetPhysicalMemoryInfo(
		LPTSTR lpszManufacturer,
		DWORD cbManufacturer,
		LPTSTR lpszSerialNumber,
		DWORD cbSerialNumber,
		LPTSTR lpszPartNumber,
		DWORD cbPartNumber,
		IWbemServices* pSvc = NULL
		);

	// Network Adapter Information (MAC)
	BIU_API
	BOOL
	GetNetworkAdapterInfo(
		LPTSTR lpszMac,
		DWORD cbMac
		);

	/*
		U should init COM & COM Security before call this function.
	
		InitializeWMI is a helper function for this situation.
	*/
	BIU_API
	BOOL 
	CreateMachineUniqueID(
		LPDWORD aIDType,
		DWORD cbIDType,
		LPTSTR lpszUniqueID,
		DWORD cbUniqueID
		);

	/*
		This function checks WMI Service first. (Need COM & COM Security)
	*/
	BIU_API
	BOOL 
	CreateMachineDefaultUniqueID(
		LPTSTR lpszUniqueID,
		DWORD cbUniqueID
		);

	BIU_API
	BOOL
	CreateMachineDefaultUniqueIDForUser(
		LPTSTR lpszUniqueID,
		DWORD cbUniqueID
		);


}


#ifdef __cplusplus
}
#endif



#endif // _BMC_H_
