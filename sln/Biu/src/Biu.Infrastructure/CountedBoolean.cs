using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu
{
    public sealed class CountedBoolean
    {
        private bool _val;
        private int _ref;
        
        public CountedBoolean True()
        {
            _ref++;
            _val = true;

            return this;
        }

        public CountedBoolean False()
        {
            if (--_ref <= 0)
            {
                _ref = 0;
                _val = false;
            }

            return this;
        }
        
        public static implicit operator bool(CountedBoolean rhs) => rhs._val;
    }
}
