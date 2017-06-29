using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Threading;
using Biu.Transmission;
using Biu.Transmission.Management;
using Biu.Threading;

namespace Biu.Transmission.Ready
{
    public class AppClient
    {        
        public uint Cookie
        {
            get { return _cookie; }
        }
        private uint _cookie;

        public bool IsConnected 
        { 
            get
            {
                if (0 == _cookie)
                {
                    return false;
                }

                return BiuIoManager.Instance.NetUnit.ContainsClient(_cookie);
            }
        }

        public ClientContext ClientContext
        {
            get
            {
                if (0 == _cookie)
                {
                    return null;
                }

                return _clientContext.Value;
            }
        }
        private Lazy<ClientContext> _clientContext;


        public ConnectionCreation CC { get; set; }

        public ConnectionCreation CCData { get; set; }

        public string Name { get; set; }

        public string Category { get; set; }

        public CancellationTokenSource CTSReconnect { get; private set; }

        public object BizContext { get; set; }

        public int ReconnectInterval { get; set; } = 10000;


        public Peer Peer
        {
            get
            {
                if (0 == Cookie)
                {
                    return null;
                }

                return _peer.Value;
            }
        }
        private Lazy<Peer> _peer;

        public ClientDataPeer DataPeer
        {
            get
            {
                if (0 == Cookie)
                {
                    return null;
                }

                return _dataPeer.Value;
            }
        }
        private Lazy<ClientDataPeer> _dataPeer;        


        public AppClient()
        {
            _cookie = 0;
            this.CTSReconnect = new CancellationTokenSource();

            Recreate();
        }

        public bool Create()
        {
            if (null == this.CC)
            {
                return false;
            }

            if (0 != _cookie)
            {
                return true;
            }

            try
            {
                if (!BiuIoManager.Instance.NetUnit.Connect(this.CC, ref _cookie))
                {
                    return false;
                }

                return true;
            }
            catch
            {
                throw;
            }
        }

        public bool CreateFinal(bool createContext = false, ServiceFactory factory = null)
        {
            try
            {
                
                if (null != this.CCData && !BiuIoManager.Instance.NetUnit.ConnectData(SocketTrait.Data, _cookie))
                {                    
                    BiuIoManager.Instance.NetUnit.CloseClient(_cookie, true);
                    return false;
                }

                if (createContext)
                {
                    var context = BiuIoManager.Instance.NewClientContext(_cookie, factory);
                    if (null == context)
                    {
                        BiuIoManager.Instance.NetUnit.CloseClient(_cookie, true);
                        _cookie = 0;
                        return false;
                    }
                }

                return true;
            }
            catch
            {
                throw;
            }
        }

        public bool CreateData(int fileID)
        {
            try
            {
                if (null == this.CCData)
                {
                    return false;
                }

                this.CCData.Ref = this.Peer.ID;
                this.CCData.Var = fileID;

                uint nCookie = 0;
                return BiuIoManager.Instance.NetUnit.Connect(this.CCData, ref nCookie);
            }
            catch
            {
                throw;
            }
        }

        public Peer AcquirePeer()
        {
            if (null == this.CC)
            {
                return null;
            }

            if (0 != _cookie)
            {
                return null;
            }

            try
            {
                return BiuIoManager.Instance.NetUnit.AcquirePeerForConnecting(this.CC, ref _cookie);
            }
            catch
            {
                throw;
            }
        }

        public bool CreateByAcquiredPeer(Peer peer, bool createContext = false, ServiceFactory factory = null)
        {
            if (null == this.CC || null == peer)
            {
                return false;
            }

            try
            {
                if (!BiuIoManager.Instance.NetUnit.Connect(peer, this.CC, ref _cookie))
                {
                    return false;
                }

                if (createContext)
                {
                    var context = BiuIoManager.Instance.NewClientContext(_cookie, factory);
                    if (null == context)
                    {
                        BiuIoManager.Instance.NetUnit.CloseClient(_cookie, true);
                        _cookie = 0;
                        return false;
                    }
                }

                return true;
            }
            catch
            {
                throw;
            }
        }

        public void Close(bool clearCC = false, bool cleatContext = false)
        {
            if (!this.CTSReconnect.Token.WaitHandle.WaitOne(0))
            {
                this.CTSReconnect.Cancel();
                this.CTSReconnect.Token.WaitHandle.WaitOne(5000);
            }

            if (0 == Cookie)
            {
                return;
            }

            BiuIoManager.Instance.NetUnit.CloseClient(_cookie, true);
            if (cleatContext)
            {
                BiuIoManager.Instance.HostContext.RemoveClient(_cookie);
            }

            Clear(clearCC);
        }

        public void Clear(bool clearCC = false)
        {
            _cookie = 0;
            Recreate();
            if (clearCC)
            {
                this.CC = null;
                this.CCData = null;
            }
        }
        
        public void Reconnect()
        {         
            Clear();
            var task = Task.Delay(this.ReconnectInterval, this.CTSReconnect.Token);
            task.ContinueWith((argTask) => Create());
        }

        private void Recreate()
        {
            _peer = new Lazy<Peer>(() => BiuIoManager.Instance.NetUnit.GetClient(_cookie));
            _dataPeer = new Lazy<ClientDataPeer>(() => BiuIoManager.Instance.NetUnit.GetClientData(_cookie));
            _clientContext = new Lazy<ClientContext>(() =>  BiuIoManager.Instance.HostContext.GetClientContext(_cookie));
        }

        public T GetUserContext<T>(ulong key)
            where T : PeerContext
        {
            if (0 == _cookie)
            {
                return null;
            }

            try
            {
                return this.ClientContext?.PeerContext as T;
            }
            catch
            {
                throw;
            }
        }
    }
}
