using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Object.Notify
{
    /// <summary>
    /// A memento to store a object's value-changing.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class PropertyOnceMemento<T>
    {
        public PropertyOnceMemento()
        {
        }

        public void Reset()
        {
            this.IsChanged = false;
        }

        public T Restore()
        {
            this.IsChanged = false;        

            return _value;
        }

        public T Value 
        {
            get
            {
                return _value;
            }
            set
            {
                if (this.IsChanged)
                {
                    return;
                }

                _value = value;
                this.IsChanged = true;
            }
        }
        protected T _value;

        public bool IsChanged { get; set; }
    }
}
