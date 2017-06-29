
#include "stdafx.h"
#include "blm.h"
#include <mscoree.h>


using namespace Biu;


BManagedLibManager::BManagedLibManager()
: m_bSucc(false)
, m_hFusion(NULL)
, m_hMscoree(NULL)
, m_lpfnLoadLibraryShim(NULL)
, m_lpfnCreateAssemblyCache(NULL)
{
}

BManagedLibManager::~BManagedLibManager()
{
}

bool BManagedLibManager::Open()
{
	m_hMscoree = LoadLibrary(_T("mscoree.dll"));
	if (!m_hMscoree)
	{
		OutputDebugString(_T("[BManagedLibManager] No mscoree.dll found\r\n"));
		return false;
	}

	m_lpfnLoadLibraryShim = (LOADLIBRARYSHIM)GetProcAddress(m_hMscoree, "LoadLibraryShim");
	if (!m_lpfnLoadLibraryShim)
	{
		return false;
	}

	//HRESULT hr = LoadLibraryShim(_T("fusion.dll"), 0, 0, &m_hFusion);
	HRESULT hr = m_lpfnLoadLibraryShim(_T("fusion.dll"), 0, 0, &m_hFusion);
	if (FAILED(hr))
	{
		return false;
	}
	
	m_lpfnCreateAssemblyCache = (CREATEASMCACHE)GetProcAddress(m_hFusion, "CreateAssemblyCache");
	if (!m_lpfnCreateAssemblyCache)
	{
		return false;
	}

	m_bSucc = SUCCEEDED(m_lpfnCreateAssemblyCache(&m_pGAC, 0));

	return m_bSucc;
}

void BManagedLibManager::Close()
{
	if (m_hMscoree)
	{
		FreeLibrary(m_hMscoree);
		m_hMscoree = NULL;
	}
}

bool BManagedLibManager::InstallAssembly(LPCTSTR lpszLibPath)
{
	if (!m_bSucc)
	{
		return false;
	}

	return SUCCEEDED(m_pGAC->InstallAssembly(0, lpszLibPath, NULL));
}

bool BManagedLibManager::UninstallAssembly(LPCTSTR lpszLibPath)
{
	if (!m_bSucc)
	{
		return false;
	}

	ULONG ulDisposition;
	return SUCCEEDED(m_pGAC->UninstallAssembly(0, lpszLibPath, NULL, &ulDisposition));
}
