using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Diagnostics
{
    public enum BiuNetDebugAction : ulong
    {
        LogRecord                       = 0x0100000001000001
    }

    public enum RecordLevel : short
    {
        Message = 10,
        Warning,
        Loophole,

        Journal = 30,

        Exception = 50
    }
}
