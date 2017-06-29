
#pragma once


#include "biu.h"
#include <fusion.h>


namespace Biu {


typedef HRESULT (__stdcall *CREATEASMCACHE)(IAssemblyCache** ppAsmCache, DWORD dwReserved);
typedef HRESULT (__stdcall *LOADLIBRARYSHIM)(LPCWSTR szDllName, LPCWSTR szVersion, LPVOID pvReserved, HMODULE *phModDll);


class BIU_API BManagedLibManager
{
public:
	BManagedLibManager();
	virtual ~BManagedLibManager();

	bool Open();
	void Close();
	bool InstallAssembly(LPCTSTR lpszLibPath);
	bool UninstallAssembly(LPCTSTR lpszLibPath);

private:
	bool m_bSucc;
	CREATEASMCACHE m_lpfnCreateAssemblyCache;
	LOADLIBRARYSHIM m_lpfnLoadLibraryShim;

	HMODULE m_hMscoree;
	HMODULE m_hFusion;
	CComPtr<IAssemblyCache> m_pGAC;
};


}
