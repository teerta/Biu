using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu
{
    public interface IFlaser
    {
        ulong Size { get; set; }

        IFlaser Next { get; set; }

        bool Write(IEnumerable<byte> buffer);

        bool Read(IEnumerable<byte> buffer, ulong size);
    }
}
