using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.WinForm.FunctionView
{
    public interface IAppFunctionView
    {
        void SetFunctionSite(AppFunctionSite site);

        bool BeforeLeaveFunction();

        void AfterLeaveFunction();

        void ViewSearch(AppFunctionViewSearchParameter search);

        void ViewLayoutMode(int modeVal);


        SearchHistory SearchHistory { get; }

        string DisplayTitle { get; set; }


        event Action<AppFunctionViewChange> NotifyViewChange;

        event Action<AppFunctionRowNotifySearchMode> NotifySearchChange;
    }
}
