using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using Biu.Transmission;
using Biu.Threading;

namespace Biu.Transmission.Management
{
    public class ServerContext
    {
        #region Member
        protected Dictionary<ulong, UserContext> _userContexts = new Dictionary<ulong,UserContext>();

        protected Dictionary<int, List<ulong>> _relation = new Dictionary<int, List<ulong>>();

        protected Dictionary<int, UserStatistics> _userStats = new Dictionary<int, UserStatistics>();

        protected object _syncRootPeer = new object();

        protected object _syncRootUser = new object();

        protected object _syncRootRelation = new object();

        protected object _syncRootUserStat = new object();
        #endregion // member


        #region Property
        public ServiceFactory Factory { get; set; }

        public ServerPeer Server { get; set; }

        public bool IsPrimaryService { get; set; }

        public Dictionary<ulong, PeerContext> PeerContexts { get; protected set; }

        public Dictionary<int, UserContext> Users { get; protected set; }

        public bool IsEmpty
        {
            get
            {
                bool bRet = false;
                lock(_syncRootPeer)
                {
                    bRet = 0 == this.PeerContexts.Count;
                }

                return bRet;
            }
        }
        #endregion // Property


        #region Protected Methods
        protected void AddRelation(int uid, ulong key)
        {
            lock(_syncRootRelation)
            {
                if (_relation.ContainsKey(uid))
                {
                    var list = _relation[uid];
                    if (!list.Contains(key))
                    {
                        list.Add(key);
                    }
                }
                else
                {
                    var list = new List<ulong>();
                    list.Add(key);
                    _relation[uid] = list;
                }
            }
        }

        protected void RemoveRelation(int uid, ulong key)
        {
            lock(_syncRootRelation)
            {
                if (_relation.ContainsKey(uid))
                {
                    var list = _relation[uid];
                    int nIdx = list.IndexOf(key);
                    if (-1 != nIdx)
                    {
                        list.RemoveAt(nIdx);
                    }
                }
            }
        }

        protected void StatLogin(int uid)
        {
            UserStatistics userStat = null;
            lock(_syncRootUserStat)
            {
                if (_userStats.ContainsKey(uid))
                {
                    userStat = _userStats[uid];
                }
                else
                {
                    userStat = this.Factory.CreateUserStatistics();
                    _userStats[uid] = userStat;
                }
            }

            userStat.RefLogin++;
        }        

        protected UserContext AddUser(int uid)
        {
            var userContext = this.Factory.CreateUserContext();
            userContext.UID = uid;
            lock(_syncRootUser)
            {
                this.Users[uid] = userContext;
            }

            return userContext;
        }

        protected UserContext RemoveUser(int uid)
        {
            var userContext = GetUserContextFromStore(uid);
            if (null == userContext)
            {
                return null;
            }

            lock(_syncRootUser)
            {
                this.Users.Remove(uid);
            }

            return userContext;
        }
        #endregion // Protected Methods


        #region Internal Methods
        internal UserContext GetUserContextFromStore(int uid)
        {
            UserContext userContext = null;
            lock (_syncRootUser)
            {
                if (this.Users.ContainsKey(uid))
                {
                    userContext = this.Users[uid];
                }
            }

            return userContext;
        }
        #endregion // Internal Methods


        #region Public Methods
        public ServerContext()
        {
            this.PeerContexts = new Dictionary<ulong, PeerContext>();
            this.Users = new Dictionary<int, UserContext>();
        }

        public PeerContext AddPeer(ulong key, UserInfo userInfo)
        {
            var peerContext = GetPeerContext(key);
            if (null != peerContext)
            {
                peerContext.Platform.SetValue(userInfo.Platform);
                return peerContext;
            }

            var userContext = GetUserContextFromStore(userInfo.UID);
            if (null == userContext)
            {
                userContext = AddUser(userInfo.UID);
            }

            userContext.SetFromUserInfo(userInfo);
            userContext.RefCount++;

            peerContext = AddPeer(key, userContext);
            peerContext.Platform.SetValue(userInfo.Platform);

            return peerContext;
        }

        public PeerContext AddPeer(ulong key, UserContext userContext)
        {
            var peerContext = GetPeerContext(key);
            if (null != peerContext)
            {
                return peerContext;
            }

            peerContext = this.Factory.CreatePeerContext();
            peerContext.Key = key;

            lock(_syncRootPeer)
            {
                this.PeerContexts[key] = peerContext;
                _userContexts[key] = userContext;
            }

            peerContext.IsLogin.SetValue(true);
            AddRelation(userContext.UID, key);
            StatLogin(userContext.UID);

            return peerContext;
        }

        public UserContext UpdatePeer(ulong key, UserInfo userInfo)
        {
            var userContext = GetUserContextFromStore(userInfo.UID);
            if (null == userContext)
            {
                userContext = AddUser(userInfo.UID);
            }

            userContext.SetFromUserInfo(userInfo);
            userContext.RefCount++;

            UserContext oldUserContext = null;
            lock(_syncRootPeer)
            {
                oldUserContext = _userContexts[key];
                oldUserContext.RefCount--;
                RemoveRelation(userInfo.UID, key);

                _userContexts[key] = userContext;

                var peerContext = this.PeerContexts[key];
                peerContext.Platform.SetValue(userInfo.Platform);
                peerContext.IsLogin.SetValue(true);
                AddRelation(userInfo.UID, key);
            }

            if (null != oldUserContext && 0 == oldUserContext.RefCount)
            {
                RemoveUser(userInfo.UID);
            }

            return userContext;
        }

        public void RemovePeer(ulong key)
        {
            PeerContext peerContext;
            UserContext userContext;
            RemovePeer(key, out peerContext, out userContext);
        }

        public void RemovePeer(ulong key, out PeerContext removedPeerContext, out UserContext removedUserContext)
        {
            removedPeerContext = null;
            removedUserContext = null;
            var peerContext = GetPeerContext(key);
            if (null == peerContext)
            {
                return;
            }

            peerContext.IsLogin.SetValue(false);

            var userContext = GetUserContext(key);
            if (null == userContext)
            {
                return;
            }

            RemoveRelation(userContext.UID, key);
            userContext.RefCount--;

            lock (_syncRootUser)
            {
                this.PeerContexts.Remove(key);
                _userContexts.Remove(key);
            }

            if (0 == userContext.RefCount)
            {
                RemoveUser(userContext.UID);
            }

            removedPeerContext = peerContext;
            removedUserContext = userContext;
        }

        public PeerContext GetPeerContext(ulong key)
        {
            PeerContext peerContext = null;
            lock(_syncRootPeer)
            {
                if (this.PeerContexts.ContainsKey(key))
                {
                    peerContext = this.PeerContexts[key];
                }
            }

            return peerContext;
        }

        public UserContext GetUserContext(ulong key)
        {
            UserContext userContext = null;
            lock(_syncRootPeer)
            {
                if (_userContexts.ContainsKey(key))
                {
                    userContext = _userContexts[key];
                }
            }

            return userContext;
        }

        public bool IsPeerLogin(ulong key)
        {
            PeerContext peerContext = GetPeerContext(key);
            if (null == peerContext)
            {
                return false;
            }

            return peerContext.IsLogin;
        }

        public bool IsUserExist(int uid)
        {
            bool bRet = false;
            lock(_syncRootUser)
            {
                bRet = this.Users.ContainsKey(uid);
            }

            return bRet;
        }

        public bool GetRelation(int uid, ref List<ulong> relations)
        {
            bool bRet = false;
            relations.Clear();
            lock(_syncRootRelation)
            {
                if (_relation.ContainsKey(uid))
                {
                    bRet = true;

                    relations.AddRange(_relation[uid]);
                }
            }

            return bRet;
        }

        public bool HasRelation(int uid)
        {
            bool bRet = false;
            lock (_syncRootRelation)
            {
                if (_relation.ContainsKey(uid) && 0 != _relation[uid].Count)
                {
                    bRet = true;
                }
            }

            return bRet;
        }

        public UserContext GetRelationUserContext(int uid)
        {
            if (!HasRelation(uid))
            {
                return null;
            }

            return GetUserContextFromStore(uid);
        }

        public bool HasUserAppeared(int uid)
        {
            bool bRet = false;
            lock (_syncRootUserStat)
            {
                if (_userStats.ContainsKey(uid))
                {
                    bRet = true;
                }
            }

            return bRet;
        }

        public void ForEachPeer(Action<PeerContext> action)
        {
            try
            {
                lock (_syncRootPeer)
                {
                    foreach (var pair in this.PeerContexts)
                    {
                        action(pair.Value);
                    }
                }
            }
            catch
            {
                throw;
            }
        }

        public void ForEachUser(Action<UserContext> action)
        {
            try
            {
                lock (_syncRootUser)
                {
                    foreach (var pair in this.Users)
                    {
                        action(pair.Value);
                    }
                }
            }
            catch
            {
                throw;
            }
        }
        #endregion // Public Methods
    }
}
