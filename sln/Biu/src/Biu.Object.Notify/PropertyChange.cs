using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Biu.Object.Notify
{
    public class PropertyChangingCancelEventArgs : PropertyChangingEventArgs
    {
        public bool Cancel { get; set; }

        public PropertyChangingCancelEventArgs(string propertyName)
            : base(propertyName)
        {
        }
    }

    public class PropertyChangingCancelEventArgs<T> : PropertyChangingCancelEventArgs
    {
        public T OriginalValue { get; private set; }

        public T NewValue { get; private set; }

        public PropertyChangingCancelEventArgs(string propertyName, T originalValue, T newValue)
            : base(propertyName)
        {
            this.OriginalValue = originalValue;
            this.NewValue = newValue;
        }
    }

    public class PropertyChangedEventArgs<T> : System.ComponentModel.PropertyChangedEventArgs
    {
        public T PreviousValue { get; private set; }

        public T CurrentValue { get; private set; }

        public PropertyChangedEventArgs(string propertyName, T previousValue, T currentValue)
            : base(propertyName)
        {
            this.PreviousValue = previousValue;
            this.CurrentValue = currentValue;
        }
    }

    public class PropertyChangedEventArgs : System.ComponentModel.PropertyChangedEventArgs
    {
        public object PreviousValue { get; private set; }

        public object CurrentValue { get; private set; }

        public PropertyChangedEventArgs(string propertyName, object previousValue, object currentValue)
            : base(propertyName)
        {
            this.PreviousValue = previousValue;
            this.CurrentValue = currentValue;
        }
    }
}
