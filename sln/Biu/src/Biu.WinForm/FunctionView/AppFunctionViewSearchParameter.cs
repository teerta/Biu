using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.WinForm.FunctionView
{
    public class AppFunctionViewSearchParameter
    {
        public AppFunctionViewSearchAction Action { get; set; }

        public long Index { get; set; }

        public long ID { get; set; }

        public string Key { get; set; }

        public object Tag { get; set; }
    }
}
