using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace Biu.Threading
{
    public class AtomicLong
    {
        private long _v = 0;


        public AtomicLong()
        {
        }

        public AtomicLong(long v)
        {
            Interlocked.Exchange(ref _v, v);
        }

        public long GetValue()
        {
            return Interlocked.CompareExchange(ref _v, _v, _v);
        }

        public void SetValue(long v)
        {
            Interlocked.Exchange(ref _v, v);
        }

        public static AtomicLong operator +(AtomicLong v1, AtomicLong v2)
        {
            long l1 = v1.GetValue();
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicLong operator +(AtomicLong v1, long v2)
        {
            long l1 = v1.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 + v2);

            return vn;
        }

        public static AtomicLong operator +(long v1, AtomicLong v2)
        {
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(v1 + v2);

            return vn;
        }

        public static AtomicLong operator -(AtomicLong v1, AtomicLong v2)
        {
            long l1 = v1.GetValue();
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicLong operator -(AtomicLong v1, long v2)
        {
            long l1 = v1.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 - v2);

            return vn;
        }

        public static AtomicLong operator -(long v1, AtomicLong v2)
        {
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(v1 - v2);

            return vn;
        }

        public static AtomicLong operator *(AtomicLong v1, AtomicLong v2)
        {
            long l1 = v1.GetValue();
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicLong operator *(AtomicLong v1, long v2)
        {
            long l1 = v1.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 * v2);

            return vn;
        }

        public static AtomicLong operator *(long v1, AtomicLong v2)
        {
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(v1 * v2);

            return vn;
        }

        public static AtomicLong operator /(AtomicLong v1, AtomicLong v2)
        {
            long l1 = v1.GetValue();
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicLong operator /(AtomicLong v1, long v2)
        {
            long l1 = v1.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l1 / v2);

            return vn;
        }

        public static AtomicLong operator /(long v1, AtomicLong v2)
        {
            long l2 = v2.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(v1 / v2);

            return vn;
        }

        public static AtomicLong operator ++(AtomicLong v)
        {
            long l = v.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l + 1);

            return vn;
        }

        public static AtomicLong operator --(AtomicLong v)
        {
            long l = v.GetValue();

            AtomicLong vn = new AtomicLong();
            vn.SetValue(l - 1);

            return vn;
        }

        public static bool operator ==(AtomicLong v1, AtomicLong v2)
        {
            long l1 = v1.GetValue();
            long l2 = v2.GetValue();

            return l1 == l2;
        }

        public static bool operator ==(AtomicLong v1, long v2)
        {
            long l1 = v1.GetValue();

            return l1 == v2;
        }

        public static bool operator ==(long v1, AtomicLong v2)
        {
            long l2 = v2.GetValue();

            return v1 == l2;
        }

        public static bool operator !=(AtomicLong v1, AtomicLong v2)
        {
            long l1 = v1.GetValue();
            long l2 = v2.GetValue();

            return l1 != l2;
        }

        public static bool operator !=(AtomicLong v1, long v2)
        {
            long l1 = v1.GetValue();

            return l1 != v2;
        }

        public static bool operator !=(long v1, AtomicLong v2)
        {
            long l2 = v2.GetValue();

            return v1 != l2;
        }

        public static implicit operator long (AtomicLong v)
        {
            return v.GetValue();
        }


        public override bool Equals(object obj)
        {
            AtomicLong atomic = obj as AtomicLong;
            if (null == atomic)
            {
                return false;
            }

            return this == atomic;
        }

        public override int GetHashCode()
        {
            long l = GetValue();
            return l.ToString().GetHashCode();
        }

        public override string ToString()
        {
            long l = GetValue();
            return l.ToString();
        }
    }
}
