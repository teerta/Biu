using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Transmission.Management
{
    public class HostFactory
    {
        public HostFactory()
        {
            
        }

        public virtual ServerContext CreateServerContext()
        {
            return new ServerContext();
        }

        public virtual ClientContext CreateClientContext()
        {
            return new ClientContext();
        }
    }
}
