using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Util
{
    public abstract class IndicatorPart
    {
        public int Level { get; set; }

        public IndicatorFullAction FullAction { get; set; }

        public long Capacity { get; set; }

        public abstract string GeneratePart();
    }
}
