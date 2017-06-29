using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace Biu.Threading
{
    public class Atomic<T>
        where T : class
    {
        private T _v;


        public Atomic()
        {
            _v = default(T);
        }

        public T GetValue()
        {
            return Interlocked.CompareExchange<T>(ref _v, _v, _v);
        }

        public void SetValue(T v)
        {
            Interlocked.Exchange(ref _v, v);
        }

        public static implicit operator T(Atomic<T> v)
        {
            return v.GetValue();
        }
    }
}
