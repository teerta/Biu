using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Threading;


namespace Biu.Transmission.Management
{
    public class UserContext
    {
        public int UID { get; set; }
        public AtomicLong RefCount { get; set; }

        public string UserName { get; set; }


        public UserContext()
        {
            this.RefCount = new AtomicLong();
        }

        public virtual void SetFromUserInfo(UserInfo userInfo)
        {
            this.UID = userInfo.UID;
            this.UserName = userInfo.UserName;
        }
    }
}
