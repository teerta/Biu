using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Util
{
    public delegate bool ParseValue<T>(string s, out T val);
}
