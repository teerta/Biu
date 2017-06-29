using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Biu.WinForm.Drawing
{
    public partial class DrawablePanel : Panel
    {
        public DrawablePanel()
        {
            InitializeComponent();

            this.DoubleBuffered = true;
        }
    }
}
