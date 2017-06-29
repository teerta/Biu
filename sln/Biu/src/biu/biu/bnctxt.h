
#ifndef _BNCTXT_H_
#define _BNCTXT_H_


#include "biu.h"
#include <string>
#include <memory>
#include <atomic>
#include <algorithm>
#include <list>
#include <map>
#include <tuple>
#include "bsyncprim.h"
#include "bpeer.h"
#include "bpeersvr.h"
#ifdef _WINDOWS
#include <atlstr.h>
#endif


using namespace std;
using namespace Biu;


namespace Biu {
	
	
	// Forwarding

	class BNHostFactory;
	class BNServiceFactory;
	class BPeerRepresentation;
	class BServerPeer;
	
	
	// Class BNUserInfo

	class BIU_API BNUserInfo
	{
	public:
		BNUserInfo();
		virtual ~BNUserInfo();

		void SetUserName(LPCTSTR lpsz);
		void GetUserName(CString& str);

		void SetPassword(LPCTSTR lpsz);
		void GetPassword(CString& str);

		inline void SetUID(int nUID) { m_nUID = nUID; }
		inline int GetUID() { return m_nUID; }

		inline void SetPlatform(unsigned int ui) { m_uiPlatform = ui; }
		inline unsigned int GetPlatform() { return m_uiPlatform; }

	protected:
		int					m_nUID;
		unsigned int		m_uiPlatform;

	private:
	#ifdef _WINDOWS
		CString				m_strUserName;
		CString				m_strPassword;
	#elif defined(__linux__) || (__APPLE__)
		string              m_strUserName;
		string              m_strPassword;
	#endif
	};


	// Class BNUserContext

	class BIU_API BNUserContext
	{
	public:
		BNUserContext();
		virtual ~BNUserContext();

		virtual void SetFromUserInfo(BNUserInfo& userInfo);

#ifdef _WINDOWS
		void GetUserName(CString& str);
		void SetUserName(LPCTSTR lpsz);
#endif

		inline const int GetUID() const { return m_lUID; }
		inline void SetUID(const int lUID) { m_lUID = lUID; }

#ifndef _WINDOWS
		inline const string& GetUserName() const { return m_strUserName; }
		inline void SetUserName(const string& strUserName) { m_strUserName = strUserName; }
#endif

		inline atomic_int& GetRefCount() { return m_lRefCount; }

	protected:
		int				m_lUID;
		atomic_int		m_lRefCount;

	#ifdef _WINDOWS
		CString			m_strUserName;
	#elif defined(__linux__) || (__APPLE__)
		string          m_strUserName;
	#endif
	};


	// Class BNPeerContext

	class BIU_API BNPeerContext
	{
	public:
		BNPeerContext();
		virtual ~BNPeerContext();

		inline BPeerRepresentation* GetRepresentation() const { return m_pRepresentation; }
		inline void SetRepresentation(BPeerRepresentation* pRepresentation) { m_pRepresentation = pRepresentation; }

		inline atomic_int& GetNumberOfInvalidCmd() { return m_dwNumberOfInvalidCmd; }
		inline atomic_bool& GetLoginStatus() { return m_fIsLogin; }
		inline atomic_uint& GetPlatform() { return m_dwPlatform; }
		inline atomic_uint64_t& GetLastHeartBeating() { return m_nLastHeartBeating; }

	protected:
		BCriticalSection							m_csFile;
		BCriticalSection							m_csBranch;
		
		BPeerRepresentation*						m_pRepresentation;	
		atomic_int									m_dwNumberOfInvalidCmd;
		atomic_bool									m_fIsLogin;
		atomic_uint									m_dwPlatform;
		atomic_uint64_t								m_nLastHeartBeating;
	};


	// Class BNUserStatistics

	class BIU_API BNUserStatistics
	{
	public:
		BNUserStatistics();
		~BNUserStatistics();

		inline void IncLogin() { ++m_lRefLogin; }
		inline int LoginCount() { return m_lRefLogin; }

	private:
		atomic_int m_lRefLogin;
	};


	// Class BNServerContext

	class BIU_API BNServerContext
	{
	public:
		friend class BNHostContext;

		typedef pair<BPeerRepresentation*, BNPeerContext*> PeerPair;

		BNServerContext();
		virtual ~BNServerContext();

		BNPeerContext* AddPeer(BPeerRepresentation* pRepresentation, BNUserInfo& userInfo);
		BNPeerContext* AddPeer(BPeerRepresentation* pRepresentation, BNUserContext* puserc = NULL);
		BNUserContext* UpdatePeer(BPeerRepresentation* pRepresentation, BNUserInfo& userInfo);
		void RemovePeer(BPeerRepresentation* pRepresentation, BNPeerContext** ppPeer = NULL, BNUserContext** ppUser = NULL);

		BNPeerContext* GetPeerContext(BPeerRepresentation* pRepresentation);
		BNUserContext* GetUserContext(BPeerRepresentation* pRepresentation);	

		bool IsPeerLogin(BPeerRepresentation* pRepresentation);
		bool IsUserExist(int nUID);
		bool GetRelation(int nUID, list<BPeerRepresentation*>& pRepresentations);
		bool HasRelation(int nUID);
		bool HasUserAppeared(int nUID);
		bool IsEmpty();

		template<class Func>
		void ForEachPeer(Func func)
		{
			BScopeLock sl(&m_csContext);
			for_each(begin(m_mapPeerContext), end(m_mapPeerContext), func);
		}

		inline BServerPeer* GetServerPeer() const { return m_pServer; }
		inline void SetServerPeer(BServerPeer* pServer) { m_pServer = pServer; }

		inline void SetContextFactory(BNServiceFactory* pFactory) { m_pContextFactory = pFactory; }
		inline BNServiceFactory* GetContextFactory() { return m_pContextFactory; }

		inline bool IsPrimaryService() { return m_fIsPrimaryService; }
		inline void SetPrimaryService(bool isPrimaryService) { m_fIsPrimaryService = isPrimaryService; }		

	protected:
		BNServiceFactory*										m_pContextFactory;
		BServerPeer*											m_pServer;
		bool													m_fIsPrimaryService;

		BCriticalSection										m_csContext;
		BCriticalSection										m_csUser;
		BCriticalSection										m_csRelation;
		BCriticalSection										m_csUserStat;

		map<BPeerRepresentation*, BNPeerContext*>				m_mapPeerContext;
		map<BPeerRepresentation*, BNUserContext*>				m_mapUserContext;
		map<int, BNUserContext*>								m_mapUser;
		map<int, list<BPeerRepresentation*>* >					m_mapRelation;
		map<int, BNUserStatistics*>								m_mapUserStat;


		BNUserContext* AddUser(int nUID);
		void RemoveUser(int nUID, BNUserContext** ppUser = NULL);
		BNUserContext* GetUserContextFromStore(int nUID);
		void AddRelation(int nUID, BPeerRepresentation* pRepresentation);
		void RemoveRelation(int nUID, BPeerRepresentation* pRepresentation);
		void StatLogin(int nUID);
	};


	// Class BNClientContext

	class BIU_API BNClientContext
	{
	public:
		friend class BNHostContext;

		BNClientContext();
		virtual ~BNClientContext();

		void Create(BPeerRepresentation* pRepresentation, BNUserInfo& userInfo);

		inline BPeerRepresentation* GetRepresentation() const { return m_pRepresentation; }
		inline void SetRepresentation(BPeerRepresentation* pRepresentation) { m_pRepresentation = pRepresentation; }

		inline void SetContextFactory(BNServiceFactory* pFactory) { m_pContextFactory = pFactory; }
		inline BNServiceFactory* GetContextFactory() { return m_pContextFactory; }

		inline BNPeerContext* GetPeerContext() { return m_pPeerContext; }
		inline BNUserContext* GetUserContext() { return m_pUserContext; }
		inline BNUserStatistics* GetUserStatistics() { return m_pUserStat; }

	protected:
		BNServiceFactory*										m_pContextFactory;
		BPeerRepresentation*									m_pRepresentation;

		BNPeerContext*											m_pPeerContext;
		BNUserContext*											m_pUserContext;
		BNUserStatistics*										m_pUserStat;
	};


	// Class BNHostContext

	class BIU_API BNHostContext
	{
	public:
		typedef pair<unsigned int, BNServerContext*> ServerPair;
		typedef pair<unsigned int, unsigned int> ServerMappingPair;
		typedef pair<BPeerRepresentation*, BNClientContext*> ClientPair;


		BNHostContext();
		virtual ~BNHostContext();

		BNServerContext* operator[](const unsigned int nCookie);

		BNServerContext* AddServer(BNServiceFactory* pFactory, const unsigned int nCookie, BServerPeer* pServer, const unsigned int nRefPrimaryCookie = 0);
		BNServerContext* RemoveServer(const unsigned int nCookie);
		void RemoveAllServers();
		BNServerContext* GetServerContext(const unsigned int nCookie);

		BNClientContext* AddClient(BNServiceFactory* pFactory, BPeer* peer);
		void RemoveClient(BPeerRepresentation* pRepresentation);
		void RemoveAllClients();
		BNClientContext* GetClientContext(BPeerRepresentation* pRepresentation);


		BNPeerContext* AddServerUserFromPrimary(const unsigned int nCookie, BPeerRepresentation* pRepresentation, int nUID);

		inline void SetContextFactory(BNHostFactory* pFactory) { m_pContextFactory = pFactory; }
		inline BNHostFactory* GetContextFactory() { return m_pContextFactory; }

	protected:
		BNHostFactory*										m_pContextFactory;

		BCriticalSection									m_csServer;
		BCriticalSection									m_csClient;

		map<unsigned int, BNServerContext*>					m_mapServer;
		map<unsigned int, unsigned int>						m_mapServerMapping;
		map<BPeerRepresentation*, BNClientContext*>			m_mapClient;

		const unsigned int GetPrimaryCookie(const unsigned int nCookie);
	};


	// Class BNHostFactory

	class BIU_API BNHostFactory
	{
	public:
		BNHostFactory();
		virtual ~BNHostFactory();

		//virtual BNUserContext* CreateUserContext();
		//virtual BNPeerContext* CreatePeerContext();
		virtual BNServerContext* CreateServerContext();
		virtual BNClientContext* CreateClientContext();
		//virtual BNHostContext* CreateHostContext();
		//virtual BNUserStatistics* CreateUserStatistics();
	};


	// Class BNServiceFactory

	class BIU_API BNServiceFactory
	{
	public:
		BNServiceFactory();
		virtual ~BNServiceFactory();

		virtual BNUserContext* CreateUserContext();
		virtual BNPeerContext* CreatePeerContext();
		virtual BNUserStatistics* CreateUserStatistics();
	};


}


#endif // _BNCTXT_H_
