
#include "stdafx.h"
#include "MachineUnique.h"


using namespace Biu::Native::Security;


// MachineUnique

MachineUnique::MachineUnique()
{
}

String^ MachineUnique::DefaultHardwareID()
{
	CString str;
	if (!CreateMachineDefaultUniqueID(str.GetBuffer(Max_ID_Size), Max_ID_Size))
	{
		str.ReleaseBuffer();
		return nullptr;
	}

	str.ReleaseBuffer();

	return gcnew String(CT2W(str));
}

String^ MachineUnique::DefaultHardwareIDForUser()
{
	CString str;
	if (!CreateMachineDefaultUniqueIDForUser(str.GetBuffer(Max_ID_Size), Max_ID_Size))
	{
		str.ReleaseBuffer();
		return nullptr;
	}

	str.ReleaseBuffer();

	return gcnew String(CT2W(str));
}
