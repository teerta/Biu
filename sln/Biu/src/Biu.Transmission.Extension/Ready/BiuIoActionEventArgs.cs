using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;

namespace Biu.Transmission.Ready
{
    public class BiuIoActionEventArgs : EventArgs
    {
        public IOOperation Operation { get; set; } = IOOperation.None;

        public Peer Peer { get; set; }

        public PeerStateKeeper StateKeeper { get; private set; }


        public void Keep()
        {
            if (null != this.StateKeeper)
            {
                return;
            }

            this.StateKeeper = new PeerStateKeeper(this.Peer);
        }

        public void Restore()
        {
            if (null == this.StateKeeper)
            {
                return;
            }

            this.StateKeeper.Restore();
        }
    }


    public class BiuIoActionEventArgs<T> : BiuIoActionEventArgs
    {
        public T Tag { get; set; }
    }


    public class BiuIoHopArgs
    {
        public Peer Peer { get; set; }        
    }


    public class BiuIoHopArgs<T> : BiuIoHopArgs
    {
        public T Tag { get; set; }
    }
}
