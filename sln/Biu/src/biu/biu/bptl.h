
#ifndef _BPTL_H_
#define _BPTL_H_


#include "biu.h"
#include "bs.h"


using namespace Biu;


namespace Biu {

	
	// Forward Declaration
	
	struct BMemoryBrick;
	class BProtocolRestriction;
	class BCryptProvider;
	

	// Enum
	
	typedef enum tagB_Data_Channel_Result : unsigned int
	{
		B_DC_R_Succeeded                             = 0x00000001,
		B_DC_R_Failed,
		B_DC_R_Continue,
		B_DC_R_More,
		B_DC_R_InvalidSecurity,
		B_DC_R_DecryptFailed,
		B_DC_R_HashFailed,
		B_DC_R_SigFailed
	} B_Data_Channel_Result, *LPB_Data_Channel_Result;
	
	
	// Interface IBDataChannel
	
	struct IBDataChannel
	{
		virtual B_Data_Channel_Result ParseData(BMemoryBrick* pBrick, size_t& nDataPos, void* pSec = NULL, BProtocolRestriction* pRestriction = NULL) = 0;     
		
		virtual bool CreateObj(BMemoryBrick* pBrick, IBSerializable* pObj, void* pVal = NULL, void* pSec = NULL) = 0;
		virtual bool GetObj(IBSerializable* pObj) = 0;
		
		virtual void CreatePod(BMemoryBrick* pBrick, const void* pData, size_t cbData) = 0;
		virtual bool GetPod(void* pData, size_t& cbRead) = 0;
		virtual size_t GetPodSize() = 0;
		
		virtual IBDataChannel* Clone() = 0;
		virtual void ChangeMemory(BMemoryBrick* pBrick) = 0;
		virtual void SetMemoryModel(BMemoryShack_char* pShack, BSerializer::Read_Function func) = 0;

		virtual BSerializer::Read_Function GetReadFunc() = 0;
	};
	
	
	// Class BProtocolRestriction

	class BIU_API BProtocolRestriction
	{
	public:
		BProtocolRestriction();
		virtual ~BProtocolRestriction();

		void SetMaxPacketSize(const unsigned int ulMaxPacketSize) { m_ulMaxPacket = ulMaxPacketSize; }
		const unsigned int GetMaxPacketSize() const { return m_ulMaxPacket; }
		
		void SetMinPacketSize(const unsigned int ulMinPacketSize) { m_ulMinPacket = ulMinPacketSize; }
		const unsigned int GetMinPacketSize() const { return m_ulMinPacket; }

		void SetIdnetification(const unsigned long long ullIdentification) { m_ullIdentification = ullIdentification; }
		const unsigned long long GetIdentification() const { return m_ullIdentification; }

	protected:
		unsigned long long      m_ullIdentification;
		unsigned int			m_ulMaxPacket;
		unsigned int            m_ulMinPacket;
	};


}


#endif // _BPTL_H_
