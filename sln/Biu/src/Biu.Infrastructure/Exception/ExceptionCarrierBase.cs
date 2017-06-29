using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace Biu
{
    public abstract class ExceptionCarrierBase
    {
        protected ManualResetEvent _eventDone = null;


        public bool Running { get; protected set; }

        public bool Sent { get; protected set; }

        public string LastError { get; protected set; }


        public ExceptionCarrierBase()
        {

        }


        protected void RecreateEvent(bool bCreate)
        {
            if (null != _eventDone)
            {
                _eventDone.Set();
                _eventDone.Close();
                _eventDone = null;
            }

            if (bCreate)
            {
                _eventDone = new ManualResetEvent(false);
            }
        }

        protected void SetEvent()
        {
            if (null == _eventDone)
            {
                return;
            }

            _eventDone.Set();
        }


        public bool WaitForDone()
        {
            if (null == _eventDone)
            {
                return true;
            }

            try
            {
                return WaitForDone(Timeout.Infinite);
            }
            catch
            {
                throw;
            }
        }

        public bool WaitForDone(int timeout)
        {
            if (null == _eventDone)
            {
                return true;
            }

            try
            {
                return _eventDone.WaitOne(timeout, false);
            }
            catch
            {
                throw;
            }
        }


        public virtual void Close()
        {
            RecreateEvent(false);
        }


        public abstract bool Send(string subject, string content);

        public abstract void CancelSend();


        protected void OnSendCompleted()
        {
            if (null != SendCompleted)
            {
                SendCompleted(this, new EventArgs());
            }
        }

        public event EventHandler SendCompleted;
    }
}
