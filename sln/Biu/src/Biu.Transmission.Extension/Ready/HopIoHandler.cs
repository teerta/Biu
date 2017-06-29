using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;
using Biu.Transmission.Management;

namespace Biu.Transmission.Ready
{
    public class HopIoHandler
    {
        public string Name { get; set; }

        public IOCommand Command
        {
            get { return this.Handler.Command; }
        }

        public IOHandler Handler { get; protected set; }


        public HopIoHandler(string name, MemoryShack shack, ulong identification, uint maxPacketSize, uint minPacketSize)
        {
            try
            {
                this.Name = name;
                this.Handler = new IOHandler(identification, maxPacketSize, minPacketSize);
                this.Handler.Shack = shack;
                this.Handler.IO += Handler_IO;
            }
            catch
            {
                throw;
            }            
        }

        internal bool Handler_IO(ulong action, Peer peer, ref IOOperation operation)
        {
            try
            {
                bool bHandled = false;
                operation = TransmissionRouter.Instance.DispatchIo(this.Name, action, peer, ref bHandled);

                return bHandled;
            }
            catch
            {
                throw;
            }
        }
    }
}
