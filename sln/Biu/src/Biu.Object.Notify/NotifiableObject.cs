using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Runtime.Serialization;

namespace Biu.Object.Notify
{
    /// <summary>
    /// A INotifyPropertyChanged-implemented class with old value.
    /// </summary>
    [Serializable]
    public class NotifiableObject : INotifyPropertyChanged
    {
        #region INotifyPropertyChanged Impl.
        protected void OnNotifyPropertyChanged<T>(String info, T oldVal, T newVal)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs<T>(info, oldVal, newVal));
            }
        }

        [field:NonSerialized]
        public event PropertyChangedEventHandler PropertyChanged;
        #endregion // INotifyPropertyChanged
    }
}
