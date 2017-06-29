using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Biu.Transmission.Management;

namespace Biu.Transmission.Ready
{
    public abstract class HoppedUserControl : UserControl, ITransmissionHop
    {
        public virtual string HopName { get { return this.Name; } }


        public HoppedUserControl()
        {

        }

        ~HoppedUserControl()
        {
            TransmissionRouter.Instance.RemoveHop(this);
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

        protected override void OnParentVisibleChanged(EventArgs e)
        {            
            try
            {
                if (this.Parent.Visible)
                {
                    TransmissionRouter.Instance.AddHop(this);
                }
                else
                {
                    TransmissionRouter.Instance.RemoveHop(this);
                }

                base.OnParentVisibleChanged(e);
            }
            catch
            {
                throw;
            }
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            try
            {
                if (this.Visible)
                {
                    TransmissionRouter.Instance.AddHop(this);
                }
                else
                {
                    TransmissionRouter.Instance.RemoveHop(this);
                }

                base.OnVisibleChanged(e);
            }
            catch
            {
                throw;
            }
        }
    }
}
