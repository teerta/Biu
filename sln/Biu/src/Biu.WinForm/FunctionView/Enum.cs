using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.WinForm.FunctionView
{
    public enum AppFunctionViewSearchAction
    {
        More = 1,
        LastCommon,
        Specified
    }

    public enum AppFunctionRowNotifySearchMode
    {        
        Search = 1,
        Navigate
    }

    public enum AppContainerViewStyle
    {
        TreeSelection = 1,
        Single
    }

    [Flags]
    public enum AppFunctionViewDesiredFeatures
    {
        NavigationToggle = 0x00000001,
        Search = 0x00000002,
        LayoutMode = 0x00000004,
        Transmission = 0x00000008
    }

    [Flags]
    public enum ModuleTrait : int
    {
        None = 0x00000000,

        BiuHopIo = 0x00000001
    }
}
