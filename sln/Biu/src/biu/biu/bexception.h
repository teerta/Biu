
#include "biu.h"


#ifndef _BEXCEPTION_H_
#define _BEXCEPTION_H_


#include <exception>
#include <string>


using namespace std;


namespace Biu {


	// Class BE_NotFound

	class BE_NotFound : exception
	{
	public:
		BE_NotFound(const char *_Message) throw()
				: m_strMsg(_Message)
		{			
		}

		BE_NotFound() throw()
				: m_strMsg("Not found")
		{
		}

		virtual ~BE_NotFound() throw()
		{
		}
	
	protected:
		string m_strMsg;
	};


	class BE_InitObject : exception
	{
	public:
		BE_InitObject(const char *_Message) throw()
			: exception(_Message)
		{
		}

		BE_InitObject() throw()
			: exception("Failed: Init Object")
		{
		}

		virtual ~BE_InitObject() throw()
		{
		}
	};


}


#endif // _BEXCEPTION_H_
