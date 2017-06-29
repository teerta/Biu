using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu
{
    public interface IMound
    {
        IFlaser Pop();

        void Push(IFlaser flaser);

        IFlaser PushBreak(IFlaser flaser, IFlaser breakFlaser);
    }
}
