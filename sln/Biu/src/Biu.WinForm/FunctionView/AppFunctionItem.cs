using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Biu.WinForm.FunctionView
{
    public class AppFunctionItem
    {
        public AppFunctionItem()
        {
            this.Modules = new List<string>();
            this.Permissions = new List<string>();

            this.Items = new BindingList<AppFunctionItem>();
        }

        public string Name { get; set; }

        public string Key { get; set; }

        public bool IsModule { get; set; }

        public bool InvisibleWhenInvalidPermission { get; set; }

        public bool IsFunctionCollection { get; set; }

        public bool CheckAllPermissions { get; set; }

        public Type TargetType { get; set; }

        public string Description { get; set; }

        public int SortFactor { get; set; }

        public List<string> Modules { get; set; }

        public List<string> Permissions { get; set; }

        public BindingList<AppFunctionItem> Items { get; private set; }
    }
}
