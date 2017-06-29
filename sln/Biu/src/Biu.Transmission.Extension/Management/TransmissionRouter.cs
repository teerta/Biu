using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Biu.Transmission;

namespace Biu.Transmission.Management
{
    public interface ITransmissionHop
    {
        IOOperation HopIo(string name, ulong action, Peer peer, ref bool handled);

        void EnterHop();

        void LeaveHop();


        string HopName { get; }
    }

    public class TransmissionRouter
    {
        private ConcurrentDictionary<string, WeakReference<ITransmissionHop>> _dictHop = new ConcurrentDictionary<string, WeakReference<ITransmissionHop>>();


        public ITransmissionHop MainHop { get; set; }


        #region Singleton
        private static readonly Lazy<TransmissionRouter> _lazyInstance = 
            new Lazy<TransmissionRouter>(() => { return new TransmissionRouter(); });

        public static TransmissionRouter Instance { get { return _lazyInstance.Value; } }
        #endregion //Singleton

        private TransmissionRouter()
        {
        }

        public void AddHop(ITransmissionHop hop)
        {
            try
            {
                _dictHop.AddOrUpdate(hop.HopName, new WeakReference<ITransmissionHop>(hop),
                    (k, v) =>
                    {
                        return v;
                    });

                hop.EnterHop();
            }
            catch
            {
                throw;
            }
        }

        public void RemoveHop(ITransmissionHop hop)
        {
            try
            {
                WeakReference<ITransmissionHop> v;
                //while (!_dictHop.TryRemove(hop.HopName, out v))
                //{
                //    Thread.Sleep(50);
                //}

                if (!_dictHop.TryRemove(hop.HopName, out v))
                {
                    return;
                }

                hop.LeaveHop();
            }
            catch
            {
                throw;
            }
        }

        public bool TryRemoveHop(ITransmissionHop hop)
        {
            try
            {
                WeakReference<ITransmissionHop> v;
                return _dictHop.TryRemove(hop.HopName, out v);
            }
            catch
            {
                throw;
            }
        }

        public IOOperation DispatchIo(string name, ulong action, Peer peer, ref bool handled)
        {
            IOOperation operation = IOOperation.Read;
            Ashley ashley = peer.Protocol;
            try
            {
                bool bHandled = false;
                if (null != this.MainHop)
                {
                    operation = this.MainHop.HopIo(name, action, peer, ref handled);
                    if (handled)
                    {
                        bHandled = true;
                    }
                }

                foreach (var item in _dictHop.Values)
                {
                    ITransmissionHop hop;
                    if (!item.TryGetTarget(out hop))
                    {
                        continue;
                    }

                    operation = hop.HopIo(name, action, peer, ref handled);
                    if (handled)
                    {
                        bHandled = true;
                    }
                }

                handled = bHandled;

                return operation;
            }
            catch
            {
                throw;
            }
            finally
            {
                ashley.KillObject();
            }
        }        
    }
}
