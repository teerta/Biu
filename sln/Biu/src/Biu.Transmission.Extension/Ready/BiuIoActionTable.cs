using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;

namespace Biu.Transmission.Ready
{
    public class BiuIoActionTable
    {
        public Dictionary<string, BiuIoActionTableDisp> Dispatchings { get; protected set; }

        public BiuIoActionTable()
        {
            this.Dispatchings = new Dictionary<string, BiuIoActionTableDisp>();
        }

        public IOOperation Dispatch<T>(string name, ulong actionID, Peer peer, ref bool handled, T tag = default(T))
        {
            try
            {
                handled = false;
                if (this.Dispatchings.ContainsKey(name))
                {
                    return this.Dispatchings[name].Dispatch(actionID, peer, ref handled, tag);
                }

                return IOOperation.None;
            }
            catch
            {
                throw;
            }
        }

        public IOOperation Dispatch(string name, ulong actionID, Peer peer, ref bool handled)
        {
            try
            {
                return Dispatch<object>(name, actionID, peer, ref handled, null);
            }
            catch
            {
                throw;
            }
        }

        public BiuIoActionTableDisp Add(string name)
        {
            try
            {
                if (this.Dispatchings.ContainsKey(name))
                {
                    return this.Dispatchings[name];
                }

                var disp = new BiuIoActionTableDisp()
                {
                    HandlerName = name
                };
                this.Dispatchings[name] = disp;

                return disp;
            }
            catch
            {
                throw;
            }
        }
    }
}
