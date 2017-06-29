using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace Biu.Threading
{
    public class AtomicBool
    {
        private int _v = 0;


        private int BoolToInt(bool v)
        {
            return v ? 1 : 0;
        }

        private bool IntToBool(int n)
        {
            return 1 == n;
        }


        public AtomicBool()
        {
        }

        public AtomicBool(bool v)
        {
            int n = BoolToInt(v);
            Interlocked.Exchange(ref _v, n);
        }

        public bool GetValue()
        {
            int n = Interlocked.CompareExchange(ref _v, _v, _v);
            return IntToBool(n);
        }

        public void SetValue(bool v)
        {
            int n = BoolToInt(v);
            Interlocked.Exchange(ref _v, n);
        }

        public static AtomicBool operator !(AtomicBool v)
        {
            bool b = v.GetValue();
            return new AtomicBool(b);
        }

        public static bool operator ==(AtomicBool v1, AtomicBool v2)
        {
            bool l1 = v1.GetValue();
            bool l2 = v2.GetValue();

            return l1 == l2;
        }

        public static bool operator ==(AtomicBool v1, bool v2)
        {
            bool l1 = v1.GetValue();

            return l1 == v2;
        }

        public static bool operator ==(bool v1, AtomicBool v2)
        {
            bool l2 = v2.GetValue();

            return v1 == l2;
        }

        public static bool operator !=(AtomicBool v1, AtomicBool v2)
        {
            bool l1 = v1.GetValue();
            bool l2 = v2.GetValue();

            return l1 != l2;
        }

        public static bool operator !=(AtomicBool v1, bool v2)
        {
            bool l1 = v1.GetValue();

            return l1 != v2;
        }

        public static bool operator !=(bool v1, AtomicBool v2)
        {
            bool l2 = v2.GetValue();

            return v1 != l2;
        }

        public static implicit operator bool (AtomicBool v)
        {
            return v.GetValue();
        }


        public override bool Equals(object obj)
        {
            AtomicBool atomic = obj as AtomicBool;
            if (null == atomic)
            {
                return false;
            }

            return this == atomic;
        }

        public override int GetHashCode()
        {
            bool l = GetValue();
            return l.ToString().GetHashCode();
        }

        public override string ToString()
        {
            bool l = GetValue();
            return l.ToString();
        }
    }
}
