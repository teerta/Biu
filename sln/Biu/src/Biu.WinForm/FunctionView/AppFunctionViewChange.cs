using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.WinForm.FunctionView
{
    public class AppFunctionViewChange
    {
        public bool NeedSearch { get; set; }

        public AppFunctionViewDesiredFeatures Features { get; set; }

        public List<AppViewValuePair> LayoutModes { get; set; }


        public AppFunctionViewChange()
        {
            this.LayoutModes = new List<AppViewValuePair>();
        }

        public void AddLayoutMode(int key, string name, bool defaultMode = false)
        {
            this.LayoutModes.Add(new AppViewValuePair()
                {
                    Key = key,
                    Name = name,
                    BooleanFlag = defaultMode
                });
        }
    }
}
