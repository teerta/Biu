using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;
using Biu.Threading;

namespace Biu.Transmission.Management
{
    public class PeerContext
    {
        public ulong Key { get; set; }

        public AtomicLong NumberOfInvalidCommands { get; set; }

        public AtomicBool IsLogin { get; set; }

        public AtomicLong Platform { get; set; }

        public AtomicLong LastHeartBeating { get; set; }


        public PeerContext()
        {
            this.Key = 0;
            this.NumberOfInvalidCommands = new AtomicLong();
            this.IsLogin = new AtomicBool();
            this.Platform = new AtomicLong();
            this.LastHeartBeating = new AtomicLong();
        }
    }
}
