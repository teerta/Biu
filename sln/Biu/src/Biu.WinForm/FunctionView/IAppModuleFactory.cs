using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.WinForm.FunctionView
{
    public interface IAppModuleFactory
    {
        IAppModuleManager ModuleManager { get; }


        AppFunctionItem[] CreateModules();
    }
}
