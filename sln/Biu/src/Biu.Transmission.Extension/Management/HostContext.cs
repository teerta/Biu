using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;
using Biu.Threading;

namespace Biu.Transmission.Management
{
    public class HostContext
    {
        #region Member
        protected object _syncRootServer = new object();

        protected object _syncRootClient = new object();

        protected Dictionary<uint, ServerContext> _servers = new Dictionary<uint, ServerContext>();

        protected Dictionary<ulong, ClientContext> _clients = new Dictionary<ulong, ClientContext>();

        protected Dictionary<uint, uint> _serverMapping = new Dictionary<uint, uint>();
        #endregion // Member


        #region Property
        public HostFactory Factory { get; set; }
        #endregion // Property


        #region Protected Methods
        protected uint GetPrimaryCookie(uint cookie)
        {
            uint primaryCookie = 0;
            lock(_syncRootServer)
            {
                if (_serverMapping.ContainsKey(cookie))
                {
                    primaryCookie = _serverMapping[cookie];
                }
            }

            return primaryCookie;
        }
        #endregion // Protected Methods


        #region Public Methods
        public HostContext()
        {

        }

        public ServerContext AddServer(ServiceFactory factory, uint cookie, ServerPeer serverPeer, uint refPrimaryCookie = 0)
        {
            var serverContext = GetServerContext(cookie);
            if (null != serverContext)
            {
                return serverContext;
            }

            serverContext = this.Factory.CreateServerContext();
            serverContext.Factory = factory;
            serverContext.Server = serverPeer;

            lock(_syncRootServer)
            {
                _servers[cookie] = serverContext;
                _serverMapping[cookie] = 0 == refPrimaryCookie ? cookie : refPrimaryCookie;
            }

            return serverContext;
        }

        public ServerContext RemoveServer(uint cookie)
        {
            var serverContext = GetServerContext(cookie);
            if (null == serverContext)
            {
                return null;
            }

            bool bOK = true;
            lock(_syncRootServer)
            {
                if (serverContext.IsPrimaryService)
                {
                    bOK = !_serverMapping.Any(p =>
                        {
                            return p.Value == cookie;
                        }
                        );
                }

                if (bOK)
                {
                    _servers.Remove(cookie);
                    _serverMapping.Remove(cookie);
                }
                else 
                {
                    serverContext = null;
                }
            }

            return serverContext;
        }

        public void RemoveAllServers()
        {
            IEnumerable<uint> list = null;

            lock(_syncRootServer)
            {
                list = from s in _servers
                       select s.Key;
            }

            foreach (var item in list)
            {
                RemoveServer(item);
            }
        }

        public ServerContext GetServerContext(uint cookie)
        {
            ServerContext serverContext = null;
            lock(_syncRootServer)
            {
                if (_servers.ContainsKey(cookie))
                {
                    serverContext = _servers[cookie];
                }
            }

            return serverContext;
        }

        public ClientContext AddClient(ServiceFactory factory, Peer peer)
        {
            var clientContext = GetClientContext(peer.Key);
            if (null != clientContext)
            {
                return clientContext;
            }

            clientContext = this.Factory.CreateClientContext();
            clientContext.Factory = factory;
            clientContext.Key = peer.Key;

            lock (_syncRootServer)
            {
                _clients[peer.Key] = clientContext;
            }

            return clientContext;
        }

        public ClientContext RemoveClient(ulong key)
        {
            var clientContext = GetClientContext(key);
            if (null == clientContext)
            {
                return null;
            }

            lock (_syncRootServer)
            {
                _clients.Remove(key);
            }

            return clientContext;
        }

        public void RemoveAllClients()
        {
            IEnumerable<ulong> list = null;

            lock (_syncRootClient)
            {
                list = from c in _clients
                       select c.Key;
            }

            foreach (var item in list)
            {
                RemoveClient(item);
            }
        }

        public ClientContext GetClientContext(ulong key)
        {
            ClientContext clientContext = null;
            lock (_syncRootClient)
            {
                if (_clients.ContainsKey(key))
                {
                    clientContext = _clients[key];
                }
            }

            return clientContext;
        }

        public PeerContext AddServerUserFromPrimary(uint cookie, ulong key, int uid)
        {
            ServerContext primaryServerContext = GetServerContext(GetPrimaryCookie(cookie));
            if (null == primaryServerContext)
            {
                return null;
            }

            UserContext userContext = primaryServerContext.GetUserContextFromStore(uid);
            if (null == userContext)
            {
                return null;
            }

            ServerContext serverContext = GetServerContext(cookie);
            if (null == serverContext)
            {
                return null;
            }

            return serverContext.AddPeer(key, userContext);
        }
        #endregion // Public Methods
    }
}
