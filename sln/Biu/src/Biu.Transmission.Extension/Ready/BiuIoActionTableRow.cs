using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;

namespace Biu.Transmission.Ready
{
    public class BiuIoActionTableRow
    {
        public ulong ActionID { get; set; }

        public IOOperation Operation { get; set; }

        public BiuIoHopAction SyncAction { get; set; }

        public BiuIoActionHandler AsyncAction { get; set; }


        public BiuIoActionTableRow(ulong actionID, IOOperation operation)
        {
            if (0 == actionID)
            {
                throw new ArgumentException("Invalid actionID");
            }

            if (operation == IOOperation.None)
            {
                throw new ArgumentException("Invalid operation");
            }

            this.ActionID = actionID;
            this.Operation = operation;
        }

        public BiuIoActionTableRow(ulong actionID, BiuIoHopAction action)
        {
            if (0 == actionID)
            {
                throw new ArgumentException("Invalid actionID");
            }

            if (null == action)
            {
                throw new ArgumentNullException("Invalid action");
            }

            this.ActionID = actionID;
            this.SyncAction = action;
        }

        public BiuIoActionTableRow(ulong actionID, BiuIoActionHandler action)
        {
            if (0 == actionID)
            {
                throw new ArgumentException("Invalid actionID");
            }

            if (null == action)
            {
                throw new ArgumentNullException("Invalid action");
            }

            this.ActionID = actionID;
            this.AsyncAction = action;
        }

        public IOOperation DoAction<T>(Peer peer, ref bool handled, T tag = default(T))
        {
            try
            {
                handled = true;
                if (IOOperation.None != this.Operation)
                {
                    return this.Operation;
                }
                else if (null != this.SyncAction)
                {
                    if (null == tag)
                    {
                        var arg = new BiuIoHopArgs()
                        {
                            Peer = peer
                        };

                        return this.SyncAction(arg);
                    }
                    else
                    {
                        var arg = new BiuIoHopArgs<T>()
                        {
                            Peer = peer,
                            Tag = tag
                        };

                        return this.SyncAction(arg);
                    }
                }
                else if (null != this.AsyncAction)
                {
                    if (null == tag)
                    {
                        var arg = new BiuIoActionEventArgs()
                        {
                            Peer = peer
                        };

                        //this.AsyncAction(arg);
                        var asyncResult = this.AsyncAction.BeginInvoke(arg, null, null);
                        if (!asyncResult.CompletedSynchronously)
                        {
                            asyncResult.AsyncWaitHandle.WaitOne();
                        }
                        //arg.Restore();
                        return arg.Operation;
                    }
                    else
                    {
                        var arg = new BiuIoActionEventArgs<T>()
                        {
                            Peer = peer,
                            Tag = tag
                        };

                        this.AsyncAction(arg);
                        arg.Restore();
                        return arg.Operation;
                    }
                }

                handled = false;
                return IOOperation.None;
            }
            catch
            {
                throw;
            }
        }
    }
}
