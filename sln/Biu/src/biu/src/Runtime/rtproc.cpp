
#include "stdafx.h"
#include "brt.h"


using namespace Biu;


BProcess::BProcess()
{
	Init();
}

BProcess::BProcess(LPCTSTR lpszProcessName)
{
	Init();
	m_strProcessName = lpszProcessName;
}

BProcess::~BProcess()
{
	Close();
}

BOOL BProcess::FindProcess()
{
	Close();

	if (!GrantPrivilege())
	{
		return FALSE;
	}

	if (!m_strProcessName.GetLength())
	{
		return FALSE;
	}

	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	ZeroMemory(&pe32, sizeof(pe32));
	pe32.dwSize = sizeof(pe32);
	DWORD dwCurrentProcessID = GetCurrentProcessId();

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hProcessSnap)
	{
		return FALSE;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}

	do
	{
		if(!_tcsicmp(pe32.szExeFile, m_strProcessName))
		{
			if (dwCurrentProcessID != pe32.th32ProcessID)
			{
				m_dwProcessID = pe32.th32ProcessID;
				break;
			}
		}
	} while(Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	if (-1 == m_dwProcessID)
	{
		return FALSE;
	}

	MODULEENTRY32 me32;
	ZeroMemory(&me32, sizeof(me32));
	me32.dwSize = sizeof(me32);
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessID);
	if(INVALID_HANDLE_VALUE == hModuleSnap)
	{
		return FALSE;
	}

	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);
		return FALSE;
	}

	m_strPath = me32.szExePath;

	CloseHandle(hModuleSnap);

	return NULL != (m_hProcess= OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwProcessID));
}

void BProcess::Close()
{
	if (m_hProcessToken)
	{
		CloseHandle(m_hProcessToken);
		m_hProcessToken = NULL;
	}

	if (m_hProcess)
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}
}

BOOL BProcess::Kill(BOOL bWait)
{
	if (m_hProcess)
	{
		if (!TerminateProcess(m_hProcess, 0))
		{
			return FALSE;
		}

		if (bWait)
		{
			if (WAIT_OBJECT_0 != WaitForSingleObject(m_hProcess, 30000))
			{
				return FALSE;
			}
		}

		Close();
	}

	return TRUE;
}

BOOL BProcess::Clean()
{
	if (FindProcess())
	{
		return Kill() && DeleteFile(m_strPath);
	}

	return TRUE;
}

void BProcess::Init()
{
	m_dwProcessID = -1;
	m_hProcess = NULL;
	m_hProcessToken = NULL;
	m_strPath = _T("");
}

BOOL BProcess::GrantPrivilege()
{
	HANDLE hProcess;
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	hProcess = GetCurrentProcess();

	if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
	{
		return FALSE;
	}

	CloseHandle(hProcess);

	if (!LookupPrivilegeValue(NULL, _T("SeDebugPrivilege"), &luid))
	{
		return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{ 
		return FALSE; 
	} 

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL BProcess::OpenToken()
{
	if (!m_hProcess)
	{
		return FALSE;
	}

	return OpenProcessToken(m_hProcess, TOKEN_ALL_ACCESS, &m_hProcessToken);
}

BOOL BProcess::ImpersonateRunAsCurrentUser(LPCTSTR lpszCmdLine)
{
	const TCHAR szExplorer[] = _T("explorer.exe");

	SetProcessName(szExplorer);

	if (!FindProcess())
	{
		return FALSE;
	}

	if (!OpenToken())
	{
		return FALSE;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = _T("winsta0\\default");
	si.wShowWindow = SW_SHOW;

	if (!CreateProcessAsUser(m_hProcessToken, NULL, (LPTSTR)lpszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		return FALSE;
	}
	else
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return TRUE;
}

BOOL BProcess::Run()
{
	if (!m_strPath.GetLength())
	{
		return FALSE;
	}

	return (int)ShellExecute(NULL, _T("open"), m_strPath, NULL, NULL, SW_SHOW) > 32;
}

BOOL BProcess::RunProcess(LPCTSTR lpszCmdLine)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;

	if (!CreateProcess(NULL, (LPTSTR)lpszCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		return FALSE;
	}
	else
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return TRUE;
}
