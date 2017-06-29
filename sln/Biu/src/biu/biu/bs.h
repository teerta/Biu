
#ifndef _BSM_H_
#define _BSM_H_

#pragma warning (disable: 4251)


#include "biu.h"
#include <functional>
#include <string>
#include "bmshack.h"
#include "btype.h"
#include "bstype.h"


using namespace std;


namespace Biu {

	
	//Forward Declaration
	
	class BSerializer;
	
	
	// Interface IBSerializable
	
	struct IBSerializable
	{
		virtual bool BSerialize(bool bStoring, BSerializer& bms) = 0;                
	};
	
	
	// Class BSerializer
	
	class BIU_API BSerializer
	{
	public:
		typedef function<bool (BMemoryBrick*, char*, size_t, size_t&)> Read_Function;
		
		BSerializer();
		~BSerializer();
		
//        inline void SetLiaison(BBrickLiaison_char* p) 
//        { 
//            m_pLiaison = p;
//            m_nReadPos = 0;
//        }
		
		inline void SetShack(BMemoryShack_char* pShack) { m_pShack = pShack; }
		inline BMemoryShack_char* GetShack() { return m_pShack; }
		
		inline void SetReadFunc(Read_Function func) { m_funcRead = func; }
		inline Read_Function GetReadFunc() { return m_funcRead; }
		
		inline void SetBrick(BMemoryBrick* pBrick = NULL, size_t nReadPos = 0)
		{ 
			m_pBrick = pBrick;
			m_nReadPos = nReadPos;
		}
		
		inline void ChangeBrick(BMemoryBrick* pBrick) { m_pBrick = pBrick; }
		inline BMemoryBrick* GetBrick() { return m_pBrick; }
		
		inline void ResetReadPos() { m_nReadPos = 0; }
		inline size_t GetReadPos() { return m_nReadPos; }

		
		void Serialize(bool val);
		void Serialize(wchar_t val);
		void Serialize(char val);
		void Serialize(unsigned char val);
		void Serialize(short val);        
		void Serialize(unsigned short val);        
		void Serialize(int val);        
		void Serialize(unsigned int val);        
		void Serialize(long val);        
		void Serialize(unsigned long val);        
		void Serialize(long long val);        
		void Serialize(unsigned long long val);        
		void Serialize(float val);        
		void Serialize(double val);        
		void Serialize(long double val);
		
		void Serialize(wchar_t* pval, unsigned int cb);        // This method will consume pval's data in place.
		void Serialize(char* pval, unsigned int cb);        
		void Serialize(unsigned char* pval, unsigned int cb);
		
		void Serialize(BCabUTF8String& str);               
		
		inline void Serialize(IBSerializable& obj) { obj.BSerialize(true, *this); }
		
		inline void Serialize(const string& str)
		{
			BCabUTF8String cab = str;
			Serialize(cab);
		}

#ifdef _WINDOWS
		inline void Serialize(const CAtlStringW& str)
		{
			BCabUTF8String cab = str;
			Serialize(cab);
		}
#endif
		
		
		bool Deserialize(bool& val);
		bool Deserialize(wchar_t& val);
		bool Deserialize(char& val);
		bool Deserialize(unsigned char& val);
		bool Deserialize(short& val);
		bool Deserialize(unsigned short& val);
		bool Deserialize(int& val);
		bool Deserialize(unsigned int& val);
		bool Deserialize(long& val);
		bool Deserialize(unsigned long& val);
		bool Deserialize(long long& val);
		bool Deserialize(unsigned long long& val);
		bool Deserialize(float& val);
		bool Deserialize(double& val);
		bool Deserialize(long double& val);

		bool Deserialize(wchar_t** ppval, unsigned int* lpcb = 0, bool bAddNull = false);
		bool Deserialize(char** ppval, unsigned int* lpcb = 0, bool bAddNull = false);
		bool Deserialize(unsigned char** ppval, unsigned int* lpcb = 0);
		bool Deserialize(unsigned char* pval, unsigned int cb);
		
		bool Deserialize(BCabUTF8String& str, bool bAddNull = true);
		
		inline bool Deserialize(IBSerializable& obj) { return obj.BSerialize(false, *this); }
		
		inline bool Deserialize(string& str)
		{
			BCabUTF8String cab;
			if (Deserialize(cab))
			{
				str = cab;
				return true;
			}
			
			return false;
		}

#ifdef _WINDOWS
		inline bool Deserialize(CAtlStringW& str)
		{
			BCabUTF8String cab;
			if (Deserialize(cab))
			{
				cab.ToUnicode(str);
				return true;
			}

			return false;
		}
#endif
		
	private:
//        BBrickLiaison_char*             m_pLiaison;
		BMemoryShack_char*              m_pShack;
		BMemoryBrick*                   m_pBrick;
		size_t                          m_nReadPos;
		Read_Function                   m_funcRead;
	};
	
	
}


#endif // _BSM_H_
