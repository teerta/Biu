using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;

namespace Biu.Transmission.Ready
{
    public delegate void BiuIoActionHandler(BiuIoActionEventArgs arg);

    public delegate IOOperation BiuIoHopAction(BiuIoHopArgs arg);    
}
