
#include "stdafx.h"
#include "AlphabetEncoder.h"


using namespace Biu::Native::Security;


// AlphabetEncoder

AlphabetEncoder::AlphabetEncoder()
{
	m_pEnc = new BAlphabetEncoder();
	m_pEnc->SetDefaultAlphabet();
}

AlphabetEncoder::~AlphabetEncoder()
{
	delete m_pEnc;
}

String^ AlphabetEncoder::EncodeString(String^ str)
{
	pin_ptr<const wchar_t> pStr = PtrToStringChars(str);
	CString s = CW2T(pStr);
	LPTSTR lpsz = s.GetBuffer();
	CString ss;
	if (!m_pEnc->EncodeString(s, ss))
	{
		s.ReleaseBuffer();
		return nullptr;
	}

	s.ReleaseBuffer();

	LPTSTR lpszResult = ss.GetBuffer();
	String^ strResult = gcnew String(lpszResult);
	ss.ReleaseBuffer();

	return strResult;
}

String^ AlphabetEncoder::DecodeString(String^ str)
{
	pin_ptr<const wchar_t> pStr = PtrToStringChars(str);
	CString s = CW2T(pStr);
	LPTSTR lpsz = s.GetBuffer();
	CString ss;
	if (!m_pEnc->DecodeString(s, ss))
	{
		s.ReleaseBuffer();
		return nullptr;
	}

	s.ReleaseBuffer();

	LPTSTR lpszResult = ss.GetBuffer();
	String^ strResult = gcnew String(lpszResult);
	ss.ReleaseBuffer();

	return strResult;
}

void AlphabetEncoder::SetAlphabet(String^ str)
{
	pin_ptr<const wchar_t> pStr = PtrToStringChars(str);
	CStringA s = CW2A(pStr);
	LPCSTR lpsz = s.GetBuffer();
	m_pEnc->SetAlphabet(lpsz);
	s.ReleaseBuffer();
}

void AlphabetEncoder::ResetAlphabet()
{
	m_pEnc->SetDefaultAlphabet();
}
