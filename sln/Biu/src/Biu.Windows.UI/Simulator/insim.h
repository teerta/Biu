
#pragma once


#include <atlcoll.h>

using namespace ATL;



// Messages

#define WM_U_INPUT_SIMACTION						(WM_USER + 0x0101)


// Class CInputSimulator

class CInputSimulator
{
public:

	// Enum _InputType

	enum _InputType
	{
		Input_MouseMove = 1,
		Input_MouseMoveAbsolute,
		Input_MouseLeftDown,
		Input_MouseLeftUp,
		Input_MouseRightDown,
		Input_MouseRightUp,
		Input_MouseMiddleDown,
		Input_MouseMiddleUp,
		
		Msg_MouseLeft,
		Msg_MouseRight,
		Message,
		Msg_Command
	};

	// Enum _CommandType

	enum _CommandType
	{
		Command_Menu = 1,
		Command_Accelerator,
		Command_Control
	};

	// Class _InputParameter

	class _InputParameter
	{
	public:
		_InputParameter();
		_InputParameter(const _InputParameter& rhs);
		virtual ~_InputParameter();		

		_InputParameter& operator=(_InputParameter& rhs);

		void ToString(CString& str);

		_InputType			m_type;
		_CommandType		m_cmd;
		LONG				m_dx;
		LONG				m_dy;
		DWORD				m_mouseData;
		DWORD				m_time;
		DWORD				m_dparam;
		WORD				m_wparam1;
		WORD				m_wparam2;
		int					m_interval;
		int					m_targetID;
		CString				m_windowClassName;
		CString				m_windowTitle;
		CString				m_controlClassName;
		CString				m_controlTitle;
		bool				m_alert;
		DWORD				m_times;
		bool				m_namePattern;

	protected:
		void Copy(const _InputParameter& rhs);
	};


	struct EnumParameter
	{
		_InputParameter* pIP;
		CInputSimulator* pThis;
	};


	CInputSimulator();
	virtual ~CInputSimulator();

	bool Create();
	void Close();

	inline void SetCharacteristics(int cx, int cy);
	inline void RegisterWindowMessagePump(HWND hwnd = NULL);

	void SimulateMouseMove(LONG dx, LONG dy, DWORD dwTime = 0, bool bAbsolute = true);
	void SimulateMouseLeftClick(DWORD dwTime = 0, bool bDown = true);
	void SimulateMouseRightClick(DWORD dwTime = 0, bool bDown = true);
	void SimulateMouseMiddleClick(DWORD dwTime = 0, bool bDown = true);

	void SimluteMsgMouseLeftClick(LPCTSTR lpszWindowClassName, LPCTSTR lpszWindowTitle, LPCTSTR lpszControlClassName, LPCTSTR lpszControlTitle, 
		LONG x, LONG y, DWORD dwTimes = 1, int nInterval = 0);

	void SimluteMsgMouseRightClick(LPCTSTR lpszWindowClassName, LPCTSTR lpszWindowTitle, LPCTSTR lpszControlClassName, LPCTSTR lpszControlTitle,
		LONG x, LONG y, DWORD dwTimes = 1, int nInterval = 0);

	void SimluteMsgCommand(LPCTSTR lpszWindowClassName, LPCTSTR lpszWindowTitle, LPCTSTR lpszControlClassName, LPCTSTR lpszControlTitle,
		LONG x, LONG y, _CommandType cmd, int nTargetID, DWORD dwTimes = 1, int nInterval = 0);

	inline void WakeUp() { QueueUserAPC(APCWakeup, m_hThread, NULL); }

protected:

	static int Size_Of_Input;

	INPUT												m_input;
	CAtlList<_InputParameter>							m_listAction;
	CRITICAL_SECTION									m_csAction;
	HANDLE												m_hThread;
	HANDLE												m_hevtDone;
	SIZE												m_cOriginScreen;
	HWND												m_hwndPump;
	HWND												m_hwnd;
	HWND												m_hwndControl;
	CString												m_strWindowClassName;
	CString												m_strWindowTitle;
	CString												m_strControlClassName;
	CString												m_strControlTitle;
	bool												m_bLastWindowFound;

	static bool MatchString(bool bPattern, LPCTSTR lpszSrc, LPCTSTR lpszDest);

	static DWORD WINAPI ThreadSimulator(LPVOID lpv);
	static void CALLBACK APCWakeup(ULONG_PTR sqParam);
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);


	inline void LockAction() { EnterCriticalSection(&m_csAction); }
	inline void UnlockAction() { LeaveCriticalSection(&m_csAction); }	

	void OriginTransform(LONG& dx, LONG& dy);
	
	bool GetAction(_InputParameter& param);
	void AddAction(_InputParameter& param);

	void PumpMessage(_InputParameter& ip);

	//bool GetWindowAndControl(LPCTSTR lpszWindowClassName, LPCTSTR lpszWindowTitle, LPCTSTR lpszControlClassName, LPCTSTR lpszControlTitle, LONG x, LONG y);
	bool GetWindowAndControl(_InputParameter* pIP);
	bool GetWindow(_InputParameter* pIP);	

	bool MouseMove(LONG dx, LONG dy, DWORD dwTime = 0, bool bAbsolute = true);
	bool MouseLeftClick(DWORD dwTime = 0, bool bDown = true);
	bool MouseRightClick(DWORD dwTime = 0, bool bDown = true);
	bool MouseMiddleClick(DWORD dwTime = 0, bool bDown = true);

	LRESULT MsgMouseLeftClick(LONG x, LONG y);
	LRESULT MsgMouseRightClick(LONG x, LONG y);
	LRESULT MsgCommand(LONG x, LONG y, _CommandType cmd, int id);
};
