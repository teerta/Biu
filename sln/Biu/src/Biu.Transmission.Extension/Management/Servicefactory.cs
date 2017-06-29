using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Transmission.Management
{
    public class ServiceFactory
    {
        public ServiceFactory()
        {

        }

        public virtual PeerContext CreatePeerContext()
        {
            return new PeerContext();
        }

        public virtual UserContext CreateUserContext()
        {
            return new UserContext();
        }

        public virtual UserStatistics CreateUserStatistics()
        {
            return new UserStatistics();
        }
    }
}
