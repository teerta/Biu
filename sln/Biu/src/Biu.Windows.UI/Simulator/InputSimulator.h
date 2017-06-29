
#pragma once


#include "insim.h"
#include "Enum.h"


using namespace System;


namespace Biu { namespace Windows { namespace UI { namespace Simulator {


	public ref class InputSimulator
	{
	public:
		__clrcall InputSimulator();
		__clrcall ~InputSimulator();
		__clrcall !InputSimulator();

		void __clrcall SimluteMsgMouseLeftClick(String^ windowClassName, String^ windowTitle, String^ controlClassName, String^ controlTitle,
			int x, int y, int times, int interval);

		void __clrcall SimluteMsgMouseRightClick(String^ windowClassName, String^ windowTitle, String^ controlClassName, String^ controlTitle,
			int x, int y, int times, int interval);

		void __clrcall SimluteMsgCommand(String^ windowClassName, String^ windowTitle, String^ controlClassName, String^ controlTitle,
			int x, int y, int times, int interval, InputCommandType cmd, int targetID);

		void __clrcall WakeUp();

	private:
		CInputSimulator* m_pSim;
	};


} } } }
