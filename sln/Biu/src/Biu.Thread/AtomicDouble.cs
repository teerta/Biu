using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

namespace Biu.Threading
{
    public class AtomicDouble
    {
        private double _v = 0;


        public AtomicDouble()
        {
        }

        public AtomicDouble(double v)
        {
            Interlocked.Exchange(ref _v, v);
        }

        public double GetValue()
        {
            return Interlocked.CompareExchange(ref _v, _v, _v);
        }

        public void SetValue(double v)
        {
            Interlocked.Exchange(ref _v, v);
        }

        public static AtomicDouble operator +(AtomicDouble v1, AtomicDouble v2)
        {
            double l1 = v1.GetValue();
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicDouble operator +(AtomicDouble v1, double v2)
        {
            double l1 = v1.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 + v2);

            return vn;
        }

        public static AtomicDouble operator +(double v1, AtomicDouble v2)
        {
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(v1 + v2);

            return vn;
        }

        public static AtomicDouble operator -(AtomicDouble v1, AtomicDouble v2)
        {
            double l1 = v1.GetValue();
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicDouble operator -(AtomicDouble v1, double v2)
        {
            double l1 = v1.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 - v2);

            return vn;
        }

        public static AtomicDouble operator -(double v1, AtomicDouble v2)
        {
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(v1 - v2);

            return vn;
        }

        public static AtomicDouble operator *(AtomicDouble v1, AtomicDouble v2)
        {
            double l1 = v1.GetValue();
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicDouble operator *(AtomicDouble v1, double v2)
        {
            double l1 = v1.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 * v2);

            return vn;
        }

        public static AtomicDouble operator *(double v1, AtomicDouble v2)
        {
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(v1 * v2);

            return vn;
        }

        public static AtomicDouble operator /(AtomicDouble v1, AtomicDouble v2)
        {
            double l1 = v1.GetValue();
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 + l2);

            return vn;
        }

        public static AtomicDouble operator /(AtomicDouble v1, double v2)
        {
            double l1 = v1.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l1 / v2);

            return vn;
        }

        public static AtomicDouble operator /(double v1, AtomicDouble v2)
        {
            double l2 = v2.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(v1 / v2);

            return vn;
        }

        public static AtomicDouble operator ++(AtomicDouble v)
        {
            double l = v.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l + 1);

            return vn;
        }

        public static AtomicDouble operator --(AtomicDouble v)
        {
            double l = v.GetValue();

            AtomicDouble vn = new AtomicDouble();
            vn.SetValue(l - 1);

            return vn;
        }

        public static bool operator ==(AtomicDouble v1, AtomicDouble v2)
        {
            double l1 = v1.GetValue();
            double l2 = v2.GetValue();

            return l1 == l2;
        }

        public static bool operator ==(AtomicDouble v1, double v2)
        {
            double l1 = v1.GetValue();

            return l1 == v2;
        }

        public static bool operator ==(double v1, AtomicDouble v2)
        {
            double l2 = v2.GetValue();

            return v1 == l2;
        }

        public static bool operator !=(AtomicDouble v1, AtomicDouble v2)
        {
            double l1 = v1.GetValue();
            double l2 = v2.GetValue();

            return l1 != l2;
        }

        public static bool operator !=(AtomicDouble v1, double v2)
        {
            double l1 = v1.GetValue();

            return l1 != v2;
        }

        public static bool operator !=(double v1, AtomicDouble v2)
        {
            double l2 = v2.GetValue();

            return v1 != l2;
        }

        public static implicit operator double (AtomicDouble v)
        {
            return v.GetValue();
        }


        public override bool Equals(object obj)
        {
            AtomicDouble atomic = obj as AtomicDouble;
            if (null == atomic)
            {
                return false;
            }

            return this == atomic;
        }

        public override int GetHashCode()
        {
            double l = GetValue();
            return l.ToString().GetHashCode();
        }

        public override string ToString()
        {
            double l = GetValue();
            return l.ToString();
        }
    }
}
