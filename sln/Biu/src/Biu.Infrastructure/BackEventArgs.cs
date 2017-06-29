using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu
{
    public class BackEventArgs : EventArgs
    {
        public BackEventArgs()
            : base()
        {

        }

        public string Key { get; set; }

        public bool PreviousLevel { get; set; }
    }
}
