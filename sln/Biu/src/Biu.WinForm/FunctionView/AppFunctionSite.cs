using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Biu.WinForm.FunctionView
{
    public class AppFunctionSite
    {
        public Control Parent { get; set; }

        public Control Container { get; set; }

        public object Tag { get; set; }

        public void ShowSiteMessage(string title, string message)
        {
            if (null != this.ActionShowSiteMessage)
            {
                this.ActionShowSiteMessage(title, message);
            }
        }


        public Action<string, string> ActionShowSiteMessage { get; set; }
    }
}
