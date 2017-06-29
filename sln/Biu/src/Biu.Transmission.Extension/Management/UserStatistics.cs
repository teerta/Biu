using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Threading;

namespace Biu.Transmission.Management
{
    public class UserStatistics
    {
        public AtomicLong RefLogin { get; set; }


        public UserStatistics()
        {
            this.RefLogin = new AtomicLong();
        }
    }
}
