
#ifndef _BASOBJ_H_
#define _BASOBJ_H_


#include "biu.h"
#include "bs.h"


namespace Biu {


	// Biu_DataHeader

	class BIU_API Biu_DataHeader : public IBSerializable
	{
	public:
		Biu_DataHeader();
		virtual ~Biu_DataHeader();

		virtual bool BSerialize(bool bStoring, BSerializer& bms);

		long long					m_ID;
		unsigned long long			m_Total;
		unsigned long long			m_Current;
		unsigned int				m_BufferSize;
	};


}



#endif // _BASOBJ_H_
