using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu;
using Biu.Transmission;
using Biu.Transmission.Management;

namespace Biu.Transmission.Ready
{
    public class BiuIoManager : IDisposable
    {
        protected HostFactory _hostFactory;

        protected ServiceFactory _serviceFactory;


        public MemoryShack Shack { get; protected set; }

        public IODispatcher Dispatcher { get; protected set; }

        public NetIOExecutionUnit NetUnit { get; protected set; }

        public HostContext HostContext { get; protected set; }

        public Dictionary<string, HopIoHandler> Handlers { get; protected set; }


        #region Singleton
        private static readonly Lazy<BiuIoManager> _lazyInstance = 
            new Lazy<BiuIoManager>(() => { return new BiuIoManager(); });

        public static BiuIoManager Instance { get { return _lazyInstance.Value; } }
        #endregion //Singleton

        private BiuIoManager()
        {
            try
            {
                this.Shack = new MemoryShack();
                this.Dispatcher = new IODispatcher(this.Shack, 0, 0);
                this.NetUnit = new NetIOExecutionUnit();
                this.HostContext = new HostContext();
                this.Handlers = new Dictionary<string, HopIoHandler>();

                this.Dispatcher.AddUnit(this.NetUnit);

                InitContext();
            }
            catch
            {
                throw;
            }
        }

        ~BiuIoManager()
        {
            Dispose(false);
        }

        protected virtual void InitContext()
        {
            _hostFactory = new HostFactory();
            _serviceFactory = new ServiceFactory();

            this.HostContext.Factory = _hostFactory;
        }

        public HopIoHandler NewHandler(string name, ulong identification, uint maxPacketSize, uint minPacketSize)
        {
            try
            {
                HopIoHandler handler = new HopIoHandler(name, this.Shack, identification, maxPacketSize, minPacketSize);
                this.Handlers[name] = handler;

                return handler;
            }
            catch
            {
                throw;
            }
        }

        public void RemoveHandler(string name)
        {
            try
            {
                if (this.Handlers.ContainsKey(name))
                {
                    this.Handlers.Remove(name);
                }
            }
            catch
            {
                throw;
            }
        }

        public ServerContext NewServerContext(uint cookie, ServiceFactory factory = null)
        {
            try
            {
                return this.HostContext.AddServer(factory ?? _serviceFactory, cookie, this.NetUnit.GetServer(cookie));
            }
            catch
            {
                throw;
            }
        }

        public ClientContext NewClientContext(uint cookie, ServiceFactory factory = null)
        {
            return this.HostContext.AddClient(factory ?? _serviceFactory, this.NetUnit.GetClient(cookie));
        }

        #region IDisposable
        private bool disposed = false;

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                
            }

            this.NetUnit.Dispose();
            this.NetUnit = null;

            this.Dispatcher.Dispose();
            this.Dispatcher = null;

            disposed = true;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }
        #endregion // IDisposable
    }
}
