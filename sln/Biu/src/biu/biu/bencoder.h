
#ifndef _BENCODER_H_
#define _BENCODER_H_


#include "biu.h"
#include <string>


using namespace std;


namespace Biu {


// Class BAlphabetEncoder

class BIU_API BAlphabetEncoder
{
public:
	BAlphabetEncoder();
	~BAlphabetEncoder();

	bool SetAlphabet(const char* lpszAlphabet);
	bool SetDefaultAlphabet();

	bool EncodeString(const char* lpszData, string& str);
	bool DecodeString(const char* lpszData, string& str);

#ifdef _WINDOWS
	bool EncodeString(CAtlString& data, CAtlStringW& str);
	bool DecodeString(CAtlString& data, CAtlStringW& str);
#endif
	
	bool EncodeStringData(const char* lpszData, size_t cbData, string& str);
	bool DecodeStringData(const char* lpszData, char** ppData, size_t& cbData);

	bool EncodeData(const unsigned char* lpbData, unsigned char** lplpResult, size_t* lpcbResult);
	bool DecodeData(const unsigned char* lpbData, unsigned char** lplpResult, size_t* lpcbResult);

private:
	BAlphabetEncoder(BAlphabetEncoder& rhs);
	BAlphabetEncoder& operator=(BAlphabetEncoder& rhs);

	char*			m_lpszAlphabet;
	unsigned char*		m_lpbPageIndicators;
	unsigned char*		m_lpbReversedPageIndicators;
	char*                           m_pszAlphabetMap;
	char*			m_pszPageIndicatorMap;
	char*			m_pszReversedPageIndicatorMap;
	size_t      		m_cbAlphabet;
	size_t      		m_dwNumberOfPage;

	size_t GetEncodedDataSize(const char* lpszData);
	size_t GetDecodedDataSize(const char* lpszData);

	bool Encode(const char* lpszData, char* lpszResult, size_t cb = 0);
	bool Decode(const char* lpszData, char* lpszResult);

	void Clear();
	unsigned char GetPageIndicator(bool bClockwise, int nStart, int nPos);
	unsigned char GetPageIndicatorPos(bool bClockWise, int nStart, unsigned char data);
};


}


#endif // _BENCODER_H_
