
#include "Stdafx.h"
#include "Ashley.h"


using namespace Biu::Transmission;


// AshleyHeader

AshleyHeader::AshleyHeader()
{
	this->Base = gcnew AshleyHeader_Base();
	this->Security = gcnew AshleyHeader_Security();
	this->Recog = gcnew AshleyHeader_Recog();
	this->Part = gcnew AshleyHeader_Part();
}

AshleyHeader::~AshleyHeader()
{
}

void AshleyHeader::ToNativeHeader(LPB_ASHLEY_HEADER lpHeader)
{
	ZeroMemory(lpHeader, B_Ashley_Size_Of_AshleyHeader);
	
	lpHeader->Base.Version = (unsigned int)this->Base->Version;	
	lpHeader->Base.Flag = this->Base->Flag;

	lpHeader->Sec.Type = (unsigned short)this->Security->Alg;

	lpHeader->Recog.Identification = this->Recog->Identification;
	lpHeader->Recog.Processing = this->Recog->Processing;

	lpHeader->Pa.ID = this->Part->ID;
	lpHeader->Pa.Total = this->Part->Total;
	lpHeader->Pa.Current = this->Part->Current;
}

void AshleyHeader::FromNativeHeader(LPB_ASHLEY_HEADER lpHeader)
{
	this->Base->Version = (AshleyVersion)lpHeader->Base.Version;	
	this->Base->Flag = lpHeader->Base.Flag;

	this->Security->Alg = (AshleySecurityAlg)lpHeader->Sec.Type;

	this->Recog->Identification = lpHeader->Recog.Identification;
	this->Recog->Processing = lpHeader->Recog.Processing;

	this->Part->ID = lpHeader->Pa.ID;
	this->Part->Total = lpHeader->Pa.Total;
	this->Part->Current = lpHeader->Pa.Current;
}
