using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Util
{
    public class DateTimeWrapper
    {
        public static implicit operator DateTimeOffset(DateTimeWrapper p)
        {
            return DateTime.SpecifyKind(p._dt, DateTimeKind.Utc);
        }

        public static implicit operator DateTimeWrapper(DateTimeOffset dto)
        {
            return new DateTimeWrapper(dto.DateTime);
        }

        public static implicit operator DateTime(DateTimeWrapper dtr)
        {
            return dtr._dt;
        }

        public static implicit operator DateTimeWrapper(DateTime dt)
        {
            return new DateTimeWrapper(dt);
        }

        protected DateTimeWrapper(DateTime dt)
        {
            _dt = dt;
        }

        private readonly DateTime _dt;
    }

    public class NullableDateTimeWrapper
    {
        public static implicit operator DateTimeOffset?(NullableDateTimeWrapper p)
        {
            return p._dt.HasValue ? DateTime.SpecifyKind(p._dt.Value, DateTimeKind.Utc) : new Nullable<DateTime>();
        }

        public static implicit operator NullableDateTimeWrapper(DateTimeOffset? dto)
        {
            return new NullableDateTimeWrapper(dto.HasValue ? new Nullable<DateTime>(dto.Value.DateTime) : null);
        }

        public static implicit operator DateTime?(NullableDateTimeWrapper dtr)
        {
            return dtr._dt;
        }

        public static implicit operator NullableDateTimeWrapper(DateTime? dt)
        {
            return new NullableDateTimeWrapper(dt);
        }

        protected NullableDateTimeWrapper(DateTime? dt)
        {
            _dt = dt;
        }

        private readonly DateTime? _dt;
    }
}
