
#include "stdafx.h"
#include "InputSimulator.h"


using namespace Biu::Windows::UI::Simulator;


InputSimulator::InputSimulator()
	: m_pSim(NULL)
{
	m_pSim = new CInputSimulator();
	if (!m_pSim->Create())
	{
		throw gcnew Exception("Failed: Create Input Simulator");
	}
}

InputSimulator::~InputSimulator()
{
	this->!InputSimulator();
}

InputSimulator::!InputSimulator()
{
	if (NULL != m_pSim)
	{
		m_pSim->Close();
		delete m_pSim;
		m_pSim = NULL;
	}
}

void InputSimulator::SimluteMsgMouseLeftClick(String^ windowClassName, String^ windowTitle, String^ controlClassName, String^ controlTitle,
	int x, int y, int times, int interval)
{
	pin_ptr<const wchar_t> pszWindowClassName = PtrToStringChars(windowClassName);
	pin_ptr<const wchar_t> pszWindowTitle = PtrToStringChars(windowTitle);
	pin_ptr<const wchar_t> pszControlClassName = PtrToStringChars(controlClassName);
	pin_ptr<const wchar_t> pszControlTitle = PtrToStringChars(controlTitle);
	m_pSim->SimluteMsgMouseLeftClick(pszWindowClassName, pszWindowTitle, pszControlClassName, pszControlTitle, x, y, times, interval);
}

void InputSimulator::SimluteMsgMouseRightClick(String^ windowClassName, String^ windowTitle, String^ controlClassName, String^ controlTitle,
	int x, int y, int times, int interval)
{
	pin_ptr<const wchar_t> pszWindowClassName = PtrToStringChars(windowClassName);
	pin_ptr<const wchar_t> pszWindowTitle = PtrToStringChars(windowTitle);
	pin_ptr<const wchar_t> pszControlClassName = PtrToStringChars(controlClassName);
	pin_ptr<const wchar_t> pszControlTitle = PtrToStringChars(controlTitle);
	m_pSim->SimluteMsgMouseRightClick(pszWindowClassName, pszWindowTitle, pszControlClassName, pszControlTitle, x, y, times, interval);
}

void InputSimulator::SimluteMsgCommand(String^ windowClassName, String^ windowTitle, String^ controlClassName, String^ controlTitle,
	int x, int y, int times, int interval, InputCommandType cmd, int targetID)
{
	pin_ptr<const wchar_t> pszWindowClassName = PtrToStringChars(windowClassName);
	pin_ptr<const wchar_t> pszWindowTitle = PtrToStringChars(windowTitle);
	pin_ptr<const wchar_t> pszControlClassName = PtrToStringChars(controlClassName);
	pin_ptr<const wchar_t> pszControlTitle = PtrToStringChars(controlTitle);
	m_pSim->SimluteMsgCommand(pszWindowClassName, pszWindowTitle, pszControlClassName, pszControlTitle, 
		x, y, (CInputSimulator::_CommandType)(int)cmd, targetID, times, interval);
}

void InputSimulator::WakeUp()
{
	m_pSim->WakeUp();
}
