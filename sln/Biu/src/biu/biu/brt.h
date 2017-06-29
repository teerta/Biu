
#pragma once


#include "biu.h"


namespace Biu {


class BIU_API BProcess
{
public:
	BProcess();
	BProcess(LPCTSTR lpszProcessName);

	virtual ~BProcess();

	inline void SetProcessName(LPCTSTR lpszProcessName) { m_strProcessName = lpszProcessName; }

	inline DWORD GetProcessID() { return m_dwProcessID; }

	inline operator HANDLE() { return m_hProcess; }

	void Close();
	BOOL FindProcess();
	BOOL Kill(BOOL bWait = TRUE);
	BOOL Clean();
	BOOL OpenToken();
	BOOL ImpersonateRunAsCurrentUser(LPCTSTR lpszApp);
	BOOL RunProcess(LPCTSTR lpszCmdLine);
	BOOL Run();

protected:
	CString m_strProcessName;
	DWORD m_dwProcessID;
	HANDLE m_hProcess;
	CString m_strPath;
	HANDLE m_hProcessToken;

	void Init();
	BOOL GrantPrivilege();
};


}
