
#include "StdAfx.h"
#include "insim.h"
#include <regex>


using namespace std;


// CInputSimulator::_InputParameter

CInputSimulator::_InputParameter::_InputParameter()
{
}

CInputSimulator::_InputParameter::~_InputParameter()
{
}

CInputSimulator::_InputParameter::_InputParameter(const _InputParameter& rhs)
{
	Copy(rhs);
}

CInputSimulator::_InputParameter& CInputSimulator::_InputParameter::operator=(_InputParameter& rhs)
{
	Copy(rhs);
	return *this;
}

void CInputSimulator::_InputParameter::Copy(const _InputParameter& rhs)
{
	if (this == &rhs)
	{
		return;
	}

	m_type = rhs.m_type;
	m_cmd = rhs.m_cmd;
	m_dx = rhs.m_dx;
	m_dy = rhs.m_dy;
	m_mouseData = rhs.m_mouseData;
	m_time = rhs.m_time;
	m_dparam = rhs.m_dparam;
	m_wparam1 = rhs.m_wparam1;
	m_wparam2 = rhs.m_wparam2;
	m_interval = rhs.m_interval;
	m_targetID = rhs.m_targetID;
	m_windowClassName = rhs.m_windowClassName;
	m_windowTitle = rhs.m_windowTitle;
	m_controlClassName = rhs.m_controlClassName;
	m_controlTitle = rhs.m_controlTitle;
	m_alert = rhs.m_alert;
	m_times = rhs.m_times;
}

void CInputSimulator::_InputParameter::ToString(CString& str)
{
	switch (m_type)
	{
	case Input_MouseMove:
		str.Format(_T("Mouse Move # %d, %d"), m_dx, m_dy);
		break;

	case Input_MouseMoveAbsolute:
		str.Format(_T("Mouse Move @ %d, %d"), m_dx, m_dy);
		break;

	case Input_MouseLeftDown:
		str.Format(_T("Mouse Left Button Down"));
		break;

	case Input_MouseLeftUp:
		str.Format(_T("Mouse Left Button Up"));
		break;

	case Input_MouseRightDown:
		str.Format(_T("Mouse Right Button Down"));
		break;

	case Input_MouseRightUp:
		str.Format(_T("Mouse Right Button Up"));
		break;

	case Input_MouseMiddleDown:
		str.Format(_T("Mouse Middle Button Down"));
		break;

	case Input_MouseMiddleUp:
		str.Format(_T("Mouse Middle Button Up"));
		break;

	case Msg_MouseLeft:
		str.Format(_T("Msg Mouse Left Button"));
		break;
	}
}


// CInputSimulator

int CInputSimulator::Size_Of_Input = sizeof(INPUT);

CInputSimulator::CInputSimulator()
	: m_hThread(NULL)
	, m_hevtDone(NULL)
	, m_hwndPump(NULL)
	, m_hwnd(NULL)
	, m_hwndControl(NULL)
	, m_bLastWindowFound(false)
{
	InitializeCriticalSectionAndSpinCount(&m_csAction, 4000);

	m_cOriginScreen.cx = 0;
	m_cOriginScreen.cy = 0;
}

CInputSimulator::~CInputSimulator()
{
	DeleteCriticalSection(&m_csAction);
}

bool CInputSimulator::Create()
{
	if (m_hThread)
	{
		return false;
	}

	m_hevtDone = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hevtDone)
	{
		return false;
	}

	DWORD dwThreadID;
	m_hThread = CreateThread(NULL, 0, ThreadSimulator, this, NULL, &dwThreadID);
	
	return m_hThread;
}

void CInputSimulator::Close()
{
	if (m_hevtDone)
	{
		SetEvent(m_hevtDone);
	}

	if (m_hThread)
	{
		if (WAIT_OBJECT_0 != WaitForSingleObject(m_hThread, 10000))
		{
			TerminateThread(m_hThread, 100);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}

	if (m_hevtDone)
	{
		CloseHandle(m_hevtDone);
		m_hevtDone = NULL;
	}
}

void CInputSimulator::SetCharacteristics(int cx, int cy)
{
	m_cOriginScreen.cx = cx;
	m_cOriginScreen.cy = cy;
}

void CInputSimulator::RegisterWindowMessagePump(HWND hwnd)
{
	m_hwndPump = hwnd;
}

DWORD WINAPI CInputSimulator::ThreadSimulator(LPVOID lpv)
{
	CInputSimulator* pThis = static_cast<CInputSimulator*>(lpv);
	if (!pThis)
	{
		return 1;
	}

	_InputParameter ip;
	bool bDone = true;
	while (WAIT_OBJECT_0 != WaitForSingleObjectEx(pThis->m_hevtDone, INFINITE, TRUE))
	{
		while (pThis->GetAction(ip))
		{
			if (!pThis->GetWindowAndControl(&ip))
			{
				continue;
			}

			while (ip.m_times)
			{
				pThis->PumpMessage(ip);

				switch (ip.m_type)
				{
				case Input_MouseMove:
					pThis->OriginTransform(ip.m_dx, ip.m_dy);
					pThis->MouseMove(ip.m_dx, ip.m_dy, ip.m_time, false);
					break;

				case Input_MouseMoveAbsolute:
					pThis->OriginTransform(ip.m_dx, ip.m_dy);
					pThis->MouseMove(ip.m_dx, ip.m_dy, ip.m_time);
					break;

				case Input_MouseLeftDown:
					pThis->MouseLeftClick(ip.m_time);
					break;

				case Input_MouseLeftUp:
					pThis->MouseLeftClick(ip.m_time, false);
					break;

				case Input_MouseRightDown:
					pThis->MouseRightClick(ip.m_time);
					break;

				case Input_MouseRightUp:
					pThis->MouseRightClick(ip.m_time, false);
					break;

				case Input_MouseMiddleDown:
					pThis->MouseMiddleClick(ip.m_time);
					break;

				case Input_MouseMiddleUp:
					pThis->MouseMiddleClick(ip.m_time, false);
					break;

				case Msg_MouseLeft:
					pThis->MsgMouseLeftClick(ip.m_dx, ip.m_dy);
					break;

				case Msg_MouseRight:
					pThis->MsgMouseRightClick(ip.m_dx, ip.m_dy);
					break;

				case Msg_Command:
					pThis->MsgCommand(ip.m_dx, ip.m_dy, ip.m_cmd, ip.m_targetID);
					break;
				}

				if (ip.m_interval)
				{
					SleepEx(ip.m_interval, TRUE);
				}

				ip.m_times--;
			}
		}
	}

	return 0;
}

void CALLBACK CInputSimulator::APCWakeup(ULONG_PTR dwParam)
{
}

void CInputSimulator::OriginTransform(LONG& dx, LONG& dy)
{
	static int Max_Cord = 65535;

	if (!m_cOriginScreen.cx || !m_cOriginScreen.cy)
	{
		return;
	}

	dx = Max_Cord / m_cOriginScreen.cx * dx;
	dy = Max_Cord / m_cOriginScreen.cy * dy;
}

bool CInputSimulator::MouseMove(LONG dx, LONG dy, DWORD dwTime, bool bAbsolute)
{
	m_input.type = INPUT_MOUSE;
	m_input.mi.dx = dx;
	m_input.mi.dy = dy;
	m_input.mi.mouseData = 0;
	m_input.mi.time = dwTime;
	m_input.mi.dwFlags = MOUSEEVENTF_MOVE;
	m_input.mi.dwExtraInfo = GetMessageExtraInfo();

	if (bAbsolute)
	{
		m_input.mi.dwFlags |= MOUSEEVENTF_ABSOLUTE;
	}

	return 1 == SendInput(1, &m_input, Size_Of_Input);
}

bool CInputSimulator::MouseLeftClick(DWORD dwTime, bool bDown)
{
	m_input.type = INPUT_MOUSE;
	m_input.mi.dx = 0;
	m_input.mi.dy = 0;
	m_input.mi.mouseData = 0;
	m_input.mi.time = dwTime;
	m_input.mi.dwFlags = bDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
	m_input.mi.dwExtraInfo = GetMessageExtraInfo();
	
	return 1 == SendInput(1, &m_input, Size_Of_Input);
}

bool CInputSimulator::MouseRightClick(DWORD dwTime, bool bDown)
{
	m_input.type = INPUT_MOUSE;
	m_input.mi.dx = 0;
	m_input.mi.dy = 0;
	m_input.mi.mouseData = 0;
	m_input.mi.time = dwTime;
	m_input.mi.dwFlags = bDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
	m_input.mi.dwExtraInfo = GetMessageExtraInfo();
	
	return 1 == SendInput(1, &m_input, Size_Of_Input);
}

bool CInputSimulator::MouseMiddleClick(DWORD dwTime, bool bDown)
{
	m_input.type = INPUT_MOUSE;
	m_input.mi.dx = 0;
	m_input.mi.dy = 0;
	m_input.mi.mouseData = 0;
	m_input.mi.time = dwTime;
	m_input.mi.dwFlags = bDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
	m_input.mi.dwExtraInfo = GetMessageExtraInfo();
	
	return 1 == SendInput(1, &m_input, Size_Of_Input);
}

bool CInputSimulator::GetAction(CInputSimulator::_InputParameter& param)
{
	bool bRet = false;

	try
	{
		LockAction();

		if (m_listAction.GetCount())
		{
			param = m_listAction.RemoveHead();

			bRet = true;
		}

		UnlockAction();
	}
	catch (...)
	{
		UnlockAction();
	}

	return bRet;
}

void CInputSimulator::AddAction(CInputSimulator::_InputParameter& param)
{
	try
	{
		LockAction();

		m_listAction.AddTail(param);

		UnlockAction();
	}
	catch (...)
	{
		UnlockAction();
	}
}

void CInputSimulator::PumpMessage(_InputParameter& ip)
{
	if (!m_hwndPump)
	{
		return;
	}

	HANDLE hHeap = GetProcessHeap();
	LPBYTE lp = (LPBYTE)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(ip));
	CopyMemory(lp, &ip, sizeof(ip));
	
	PostMessage(m_hwndPump, WM_U_INPUT_SIMACTION, (WPARAM)hHeap, (LPARAM)lp);
}

void CInputSimulator::SimulateMouseMove(LONG dx, LONG dy, DWORD dwTime, bool bAbsolute)
{
	_InputParameter ip;
	ip.m_type = bAbsolute ? Input_MouseMoveAbsolute : Input_MouseMove;
	ip.m_dx = dx;
	ip.m_dy = dy;
	ip.m_time = dwTime;

	AddAction(ip);
}

void CInputSimulator::SimulateMouseLeftClick(DWORD dwTime, bool bDown)
{
	_InputParameter ip;
	ip.m_type = bDown ? Input_MouseLeftDown : Input_MouseLeftUp;
	ip.m_time = dwTime;

	AddAction(ip);
}

void CInputSimulator::SimulateMouseRightClick(DWORD dwTime, bool bDown)
{
	_InputParameter ip;
	ip.m_type = bDown ? Input_MouseRightDown : Input_MouseRightUp;
	ip.m_time = dwTime;

	AddAction(ip);
}

void CInputSimulator::SimulateMouseMiddleClick(DWORD dwTime, bool bDown)
{
	_InputParameter ip;
	ip.m_type = bDown ? Input_MouseMiddleDown : Input_MouseMiddleUp;
	ip.m_time = dwTime;

	AddAction(ip);
}

LRESULT CInputSimulator::MsgMouseLeftClick(LONG x, LONG y)
{
	LPARAM lParam = MAKELPARAM(x, y);
	SendMessage(m_hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
	SendMessage(m_hwnd, WM_LBUTTONUP, 0, lParam);

	return 0;
}

LRESULT CInputSimulator::MsgMouseRightClick(LONG x, LONG y)
{
	LPARAM lParam = MAKELPARAM(x, y);
	SendMessage(m_hwnd, WM_RBUTTONDOWN, MK_RBUTTON, lParam);
	SendMessage(m_hwnd, WM_RBUTTONUP, 0, lParam);

	return 0;
}

LRESULT CInputSimulator::MsgCommand(LONG x, LONG y, _CommandType cmd, int id)
{
	WPARAM wParam = 0;
	LPARAM lParam = 0;
	switch (cmd)
	{
	case Command_Menu:
		wParam = MAKEWPARAM(id, 0);
		break;

	case Command_Accelerator:
		wParam = MAKEWPARAM(id, 1);
		break;

	case Command_Control:
		// not-implemented
		break;

	default:
		break;
	}

	//SendMessage(m_hwndControl ? m_hwndControl : m_hwnd, WM_COMMAND, wParam, lParam);
	SendMessage(m_hwnd, WM_COMMAND, wParam, lParam);

	return 0;
}

void CInputSimulator::SimluteMsgMouseLeftClick(
	LPCTSTR lpszWindowClassName, LPCTSTR lpszWindowTitle, LPCTSTR lpszControlClassName, LPCTSTR lpszControlTitle,
	LONG x, LONG y, DWORD dwTimes, int nInterval)
{
	_InputParameter ip;
	ip.m_type = Msg_MouseLeft;
	ip.m_windowClassName = lpszWindowClassName;
	ip.m_windowTitle = lpszWindowTitle;
	ip.m_controlClassName = lpszControlClassName;
	ip.m_controlTitle = lpszControlTitle;
	ip.m_dx = x;
	ip.m_dy = y;
	ip.m_times = dwTimes;
	ip.m_interval = nInterval;

	AddAction(ip);
}

void CInputSimulator::SimluteMsgMouseRightClick(
	LPCTSTR lpszWindowClassName, LPCTSTR lpszWindowTitle, LPCTSTR lpszControlClassName, LPCTSTR lpszControlTitle,
	LONG x, LONG y, DWORD dwTimes, int nInterval)
{
	_InputParameter ip;
	ip.m_type = Msg_MouseRight;
	ip.m_windowClassName = lpszWindowClassName;
	ip.m_windowTitle = lpszWindowTitle;
	ip.m_controlClassName = lpszControlClassName;
	ip.m_controlTitle = lpszControlTitle;
	ip.m_dx = x;
	ip.m_dy = y;
	ip.m_times = dwTimes;
	ip.m_interval = nInterval;

	AddAction(ip);
}

void CInputSimulator::SimluteMsgCommand(
	LPCTSTR lpszWindowClassName, LPCTSTR lpszWindowTitle, LPCTSTR lpszControlClassName, LPCTSTR lpszControlTitle,
	LONG x, LONG y, _CommandType cmd, int nTargetID, DWORD dwTimes, int nInterval)
{
	_InputParameter ip;
	ip.m_type = Msg_Command;
	ip.m_cmd = cmd;
	ip.m_windowClassName = lpszWindowClassName;
	ip.m_windowTitle = lpszWindowTitle;
	ip.m_controlClassName = lpszControlClassName;
	ip.m_controlTitle = lpszControlTitle;
	ip.m_dx = x;
	ip.m_dy = y;
	ip.m_times = dwTimes;
	ip.m_targetID = nTargetID;
	ip.m_interval = nInterval;

	AddAction(ip);
}

bool CInputSimulator::GetWindowAndControl(_InputParameter* pIP)
{
	bool bWindow = true;
	if (m_bLastWindowFound && m_strWindowClassName == pIP->m_windowClassName && m_strWindowTitle == pIP->m_windowTitle)
	{
		bWindow = false;
		if (m_strControlClassName == pIP->m_controlClassName && m_strControlTitle == pIP->m_controlTitle)
		{
			return true;
		}
	}

	if (bWindow)
	{
		m_strWindowClassName = pIP->m_windowClassName;
		m_strWindowTitle = pIP->m_windowTitle;
		if (!GetWindow(pIP))
		{
			return false;
		}

		SetForegroundWindow(m_hwnd);
		SetActiveWindow(m_hwnd);
	}

	m_strControlClassName = pIP->m_controlClassName;
	m_strControlTitle = pIP->m_controlTitle;
	if (_T("") == m_strControlClassName)
	{
		m_hwndControl = NULL;
	}
	else
	{
		m_hwndControl = WindowFromPoint(POINT{ pIP->m_dx, pIP->m_dy });
		m_hwndControl = GetAncestor(m_hwndControl, GA_PARENT);
	}

	return true;
}

bool CInputSimulator::GetWindow(_InputParameter* pIP)
{
	EnumParameter ep{ pIP, this };
	m_bLastWindowFound = false;
	EnumWindows(EnumWindowsProc, (LPARAM)&ep);
	if (m_bLastWindowFound)
	{
		return true;
	}

	return false;
}

BOOL CInputSimulator::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	TCHAR sz[MAX_PATH] = { 0 };
	if (!GetClassName(hwnd, sz, MAX_PATH))
	{
		return TRUE;
	}

	EnumParameter* pEP = (EnumParameter*)lParam;
	_InputParameter* pIP = pEP->pIP;
	if (!MatchString(pIP->m_namePattern, pIP->m_windowClassName, sz))
	{
		return TRUE;
	}

	if (0 == GetWindowText(hwnd, sz, MAX_PATH))
	{
		return _T("") == pIP->m_windowClassName;
	}

	if (MatchString(pIP->m_namePattern, pIP->m_windowTitle, sz))
	{
		pEP->pThis->m_bLastWindowFound = true;
		pEP->pThis->m_hwnd = hwnd;
		return FALSE;
	}

	return TRUE;
}

bool CInputSimulator::MatchString(bool bPattern, LPCTSTR lpszSrc, LPCTSTR lpszDest)
{
	bool bRet = false;
	if (bPattern)
	{		
		wregex re(lpszSrc);
		const TCHAR* first = lpszDest;
		const TCHAR* last = lpszDest + _tcslen(lpszDest);
		bRet = regex_match(first, last, re);
	}
	else
	{
		bRet = !_tcsicmp(lpszSrc, lpszDest);
	}

	return bRet;
}
