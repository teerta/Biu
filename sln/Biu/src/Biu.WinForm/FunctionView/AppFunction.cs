using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Biu.WinForm.FunctionView
{
    public class AppFunction
    {
        public AppFunction()
        {
            this.Functions = new BindingList<AppFunctionItem>();
        }

        public BindingList<AppFunctionItem> Functions { get; private set; }
    }
}
