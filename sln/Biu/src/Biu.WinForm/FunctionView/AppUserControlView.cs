using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Biu.WinForm.FunctionView
{
    public class AppUserControlView : UserControl, IAppFunctionView
    {
        protected AppFunctionSite _appSite = null;
        protected SearchHistory _searchHistory = new SearchHistory();


        public virtual void SetFunctionSite(AppFunctionSite site)
        {
            _appSite = site;
        }

        public virtual bool BeforeLeaveFunction()
        {
            return true;
        }

        public virtual void AfterLeaveFunction()
        {
        }

        public virtual void ViewSearch(AppFunctionViewSearchParameter search)
        {

        }

        public virtual void ViewLayoutMode(int modeVal)
        {

        }

        protected void OnNotifyViewChange(AppFunctionViewChange change)
        {
            if (null != this.NotifyViewChange)
            {
                this.NotifyViewChange(change);
            }
        }

        protected void OnNotifySearchChange(AppFunctionRowNotifySearchMode mode)
        {
            if (null != this.NotifySearchChange)
            {
                _searchHistory.LastSearchMode = mode;
                this.NotifySearchChange(mode);
            }
        }

        public virtual SearchHistory SearchHistory
        {
            get { return _searchHistory; }
        }

        public virtual string DisplayTitle { get; set; }


        public event Action<AppFunctionViewChange> NotifyViewChange;

        public event Action<AppFunctionRowNotifySearchMode> NotifySearchChange;
    }
}
