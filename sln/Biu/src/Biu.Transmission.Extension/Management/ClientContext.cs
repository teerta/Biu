using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;
using Biu.Threading;

namespace Biu.Transmission.Management
{
    public class ClientContext
    {
        public ServiceFactory Factory { get; set; }

        public ulong Key { get; set; }

        public PeerContext PeerContext { get; protected set; }

        public UserContext UserContext { get; protected set; }

        public UserStatistics UserStatistics { get; protected set; }


        public ClientContext()
        {
            
        }

        public void Create(ulong key, UserInfo userInfo)
        {
            this.PeerContext = this.Factory.CreatePeerContext();
            this.UserContext = this.Factory.CreateUserContext();
            this.UserStatistics = this.Factory.CreateUserStatistics();

            this.UserContext.SetFromUserInfo(userInfo);
        }
    }
}
