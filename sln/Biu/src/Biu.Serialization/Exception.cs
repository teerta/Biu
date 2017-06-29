using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Serialization
{
    [Serializable]
    public class  PersistenceException : Exception
    {
        public PersistenceException()
        {

        }

        public PersistenceException(string msg)
            : base(msg)
        {

        }

        public PersistenceException(string msg, Exception innerException)
            : base(msg, innerException)
        {

        }
    }
}
