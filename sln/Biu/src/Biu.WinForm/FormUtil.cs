using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Biu.WinForm
{
    public class FormUtil
    {
        public static void ToggleControlValidation(bool enabled, Control control)
        {
            control.CausesValidation = enabled;
            if (control.HasChildren)
            {
                foreach (Control c in control.Controls)
                {
                    ToggleControlValidation(enabled, c);
                }
            }
        }
    }
}
