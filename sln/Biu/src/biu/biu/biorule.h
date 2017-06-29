
#ifndef _BIORULE_H_
#define _BIORULE_H_


#include "biu.h"
#include "biodef.h"


namespace Biu {


	// Forwarding

	class BMultipleCounter;


	// Class BIOAddressMap

	class BIU_API BIOAddressMap
	{
	public:
		typedef struct _tagBIOAddressItem
		{
			long long					Address;
			unsigned long long			Timeout;
			bool						Permanent;
		} BIOAddressItem, *LPBIOAddressItem;

		
		BIOAddressMap();
		BIOAddressMap(const BIOAddressMap& rhs) = delete;
		virtual ~BIOAddressMap();

		BIOAddressMap& operator=(const BIOAddressMap& rhs) = delete;


	protected:
		
	};



	// BIOBehaviorArbiter

	class BIU_API BIOBehaviorArbiter
	{
	public:
		BIOBehaviorArbiter();
		BIOBehaviorArbiter(const BIOBehaviorArbiter& rhs);
		virtual ~BIOBehaviorArbiter();

		BIOBehaviorArbiter& operator=(const BIOBehaviorArbiter& rhs);

		inline BMultipleCounter* GetGlbHalfOpenConnection() { return m_pGlbHalfOpenConnection; }
		inline BMultipleCounter* GetDcLessData() { return m_pDcLessData; }
		inline BMultipleCounter* GetDcFailed() { return m_pDcFailed; }
		inline BMultipleCounter* GetDcInvalidSecurity() { return m_pDcInvalidSecurity; }
		inline BMultipleCounter* GetDcDecryptFailed() { return m_pDcDecryptFailed; }
		inline BMultipleCounter* GetDcHashFailed() { return m_pDcHashFailed; }
		inline BMultipleCounter* GetDcSigFailed() { return m_pDcSigFailed; }
		inline BMultipleCounter* GetHanFailed() { return m_pHanFailed; }

	protected:
		BMultipleCounter* m_pGlbHalfOpenConnection;
		BMultipleCounter* m_pDcLessData;
		BMultipleCounter* m_pDcFailed;
		BMultipleCounter* m_pDcInvalidSecurity;
		BMultipleCounter* m_pDcDecryptFailed;
		BMultipleCounter* m_pDcHashFailed;
		BMultipleCounter* m_pDcSigFailed;
		BMultipleCounter* m_pHanFailed;

	private:
		void Copy(const BIOBehaviorArbiter& rhs);
	};


}



#endif // _BIORULE_H_
