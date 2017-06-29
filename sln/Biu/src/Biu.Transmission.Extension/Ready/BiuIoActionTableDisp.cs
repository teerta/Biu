using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Transmission.Ready
{
    public class BiuIoActionTableDisp
    {
        public string HandlerName { get; set; }

        public IOOperation DefaultOperation { get; set; } = IOOperation.None;

        public Dictionary<ulong, BiuIoActionTableRow> Rows { get; protected set; }


        public BiuIoActionTableDisp()
        {
            this.Rows = new Dictionary<ulong, BiuIoActionTableRow>();
        }

        public IOOperation Dispatch<T>(ulong actionID, Peer peer, ref bool handled, T tag = default(T))
        {
            try
            {
                if (this.Rows.ContainsKey(actionID))
                {
                    return this.Rows[actionID].DoAction(peer, ref handled, tag);                    
                }

                return IOOperation.None == this.DefaultOperation ? IOOperation.None : this.DefaultOperation;
            }
            catch
            {
                throw;
            }
        }

        public IOOperation Dispatch(ulong actionID, Peer peer, ref bool handled)
        {
            try
            {
                return Dispatch<object>(actionID, peer, ref handled);
            }
            catch
            {
                throw;
            }
        }

        public BiuIoActionTableDisp AddRow(ulong actionID, IOOperation operation)
        {
            try
            {
                if (this.Rows.ContainsKey(actionID))
                {
                    return this;
                }

                this.Rows[actionID] = new BiuIoActionTableRow(actionID, operation);

                return this;
            }
            catch
            {
                throw;
            }
        }

        public BiuIoActionTableDisp AddRow(ulong actionID, BiuIoHopAction action)
        {
            try
            {
                if (this.Rows.ContainsKey(actionID))
                {
                    return this;
                }

                this.Rows[actionID] = new BiuIoActionTableRow(actionID, action);

                return this;
            }
            catch
            {
                throw;
            }
        }

        [Obsolete("Misfunction now")]
        public BiuIoActionTableDisp AddRow(ulong actionID, BiuIoActionHandler action)
        {
            try
            {
                if (this.Rows.ContainsKey(actionID))
                {
                    return this;
                }

                this.Rows[actionID] = new BiuIoActionTableRow(actionID, action);

                return this;
            }
            catch
            {
                throw;
            }
        }
    }
}
