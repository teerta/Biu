using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.NetworkInformation;
using Biu.Transmission;
using Biu.Transmission.Management;

namespace Biu.Transmission.Ready
{
    public class AppServer
    {
        public uint Cookie
        {
            get { return _cookie; }
        }
        private uint _cookie;


        public ConnectionCreation CC { get; set; }

        public ConnectionCreation CCData { get; set; }

        public string Name { get; set; }

        public string Category { get; set; }


        public ServerPeer Server
        {
            get
            {
                if (0 == Cookie)
                {
                    return null;
                }

                try
                {                
                    return BiuIoManager.Instance.NetUnit.GetServer(_cookie);
                }
                catch
                {
                    throw;
                }                
            }
        }

        public DataPeer DataPeer
        {
            get
            {
                if (0 == Cookie)
                {
                    return null;
                }

                try
                {
                    return BiuIoManager.Instance.NetUnit.GetData(_cookie);
                }
                catch
                {
                    throw;
                }                
            }
        }

        public bool IsRunning
        {
            get
            {
                if (0 == _cookie)
                {
                    return false;
                }

                try
                {
                    var server = BiuIoManager.Instance.NetUnit.GetServer(_cookie);
                    if (null == server)
                    {
                        return false;
                    }

                    return server.IsRunning;
                }
                catch
                {
                    throw;
                }                
            }
        }

        public ServerContext ServerContext
        {
            get
            {
                if (0 == _cookie)
                {
                    return null;
                }

                try
                {
                    return BiuIoManager.Instance.HostContext.GetServerContext(_cookie);
                }
                catch
                {
                    throw;
                }                
            }
        }


        public AppServer()
        {
            _cookie = 0;
        }

        public bool Create(bool createContext = false, ServiceFactory factory = null)
        {
            if (null == this.CC)
            {
                return false;
            }

            if (0 != Cookie)
            {
                return true;
            }

            try
            {
                if (!BiuIoManager.Instance.NetUnit.Listen(this.CC, ref _cookie))
                {
                    return false;
                }

                if (null != this.CCData && !BiuIoManager.Instance.NetUnit.Listen(this.CCData, ref _cookie))
                {
                    BiuIoManager.Instance.NetUnit.CloseServer(_cookie);
                    _cookie = 0;
                    return false;
                }

                if (createContext)
                {
                    var context = BiuIoManager.Instance.NewServerContext(_cookie, factory);
                    if (null == context)
                    {
                        BiuIoManager.Instance.NetUnit.CloseServer(_cookie);
                        _cookie = 0;
                        return false;
                    }
                }

                //this.CC = null;
                //this.CCData = null;

                return true;
            }
            catch
            {
                throw;
            }
        }

        public void Close(bool clearCC = false, bool clearContext = false)
        {
            if (0 == Cookie)
            {
                return;
            }

            try
            {
                BiuIoManager.Instance.NetUnit.CloseServer(Cookie);
                if (clearContext)
                {
                    BiuIoManager.Instance.HostContext.RemoveServer(_cookie);
                }

                Clear(clearCC);
            }
            catch
            {
                throw;
            }            
        }

        public void Clear(bool clearCC = false)
        {            
            _cookie = 0;
            if (clearCC)
            {
                this.CC = null;
                this.CCData = null;
            }
        }

        public T GetUserContext<T>(ulong key)
            where T : UserContext
        {
            if (0 == _cookie)
            {
                return null;
            }

            try
            {
                return this.ServerContext?.GetUserContext(key) as T;
            }
            catch
            {
                throw;
            }            
        }

        public PeerContext GetPeerContext(int uid)
        {
            if (0 == _cookie)
            {
                return null;
            }

            try
            {
                var serverContext = this.ServerContext;
                var relations = new List<ulong>();
                if (!serverContext.GetRelation(uid, ref relations))
                {
                    return null;
                }

                return serverContext.GetPeerContext(relations.FirstOrDefault());
            }
            catch
            {
                throw;
            }
        }

        public IEnumerable<PeerContext> GetPeerContexts(int uid)
        {
            if (0 == _cookie)
            {
                return null;
            }

            try
            {
                var serverContext = this.ServerContext;
                var relations = new List<ulong>();
                if (!serverContext.GetRelation(uid, ref relations))
                {
                    return null;
                }

                var list = new List<PeerContext>();
                PeerContext peerContext = null;
                relations.ForEach(r =>
                {
                    peerContext = serverContext.GetPeerContext(r);
                    if (null != peerContext)
                    {
                        list.Add(peerContext);
                    }
                });

                return list;
            }
            catch
            {
                throw;
            }
        }

        public Peer GetClientPeer(int uid)
        {
            if (0 == _cookie)
            {
                return null;
            }

            try
            {
                var serverContext = this.ServerContext;
                var relations = new List<ulong>();
                if (!serverContext.GetRelation(uid, ref relations))
                {
                    return null;
                }

                return this.Server?.GetPeerByKey(relations.FirstOrDefault());
            }
            catch
            {
                throw;
            }
        }

        public List<Peer> GetClientPeers(int uid)
        {
            if (0 == _cookie)
            {
                return null;
            }

            try
            {
                var serverContext = this.ServerContext;
                var relations = new List<ulong>();
                if (!serverContext.GetRelation(uid, ref relations))
                {
                    return null;
                }

                var list = new List<Peer>();
                Peer peer = null;
                relations.ForEach(r =>
                {
                    peer = this.Server?.GetPeerByKey(r);
                    if (null != peer)
                    {
                        list.Add(peer);
                    }
                });

                return list;
            }
            catch
            {
                throw;
            }
        }

        public static bool IsTcpPortInUse(int port)
        {
            try
            {
                var ipGlobalProp = IPGlobalProperties.GetIPGlobalProperties();
                var connections = ipGlobalProp.GetActiveTcpConnections();
                var listeners = ipGlobalProp.GetActiveTcpListeners();
                return connections.Any(c => c.LocalEndPoint.Port == port) || listeners.Any(l => l.Port == port);
            }
            catch
            {
                throw;
            }            
        }

        public static int NextTcpFreePort(int port, IEnumerable<int> excludes = null)
        {
            try
            {
                while (true)
                {
                    if (null == excludes || (null != excludes && !excludes.Contains(port)))
                    {
                        if (!IsTcpPortInUse(port))
                        {
                            return port;
                        }
                    }

                    port++;
                }
            }
            catch
            {
                throw;
            }            
        }
    }
}
