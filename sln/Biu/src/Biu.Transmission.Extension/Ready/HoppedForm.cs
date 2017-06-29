using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Runtime.InteropServices;
using Biu.Transmission.Management;

namespace Biu.Transmission.Ready
{
    public abstract class HoppedForm : Form, ITransmissionHop
    {
        public virtual string HopName { get { return this.Name; } }


        public HoppedForm()
        {

        }


        public abstract IOOperation HopIo(string name, ulong action, Peer peer, ref bool handled);

        public abstract void EnterHop();

        public abstract void LeaveHop();


        protected override void OnLoad(EventArgs e)
        {            
            try
            {
                TransmissionRouter.Instance.AddHop(this);
                base.OnLoad(e);
            }
            catch
            {
                throw;
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);
            if (!e.Cancel)
            {
                TransmissionRouter.Instance.RemoveHop(this);
            }
        }
    }
}
