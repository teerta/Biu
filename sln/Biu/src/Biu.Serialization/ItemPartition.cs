using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Serialization
{
    /// <summary>
    /// Part of data (partition) whom PersistencePlan creates.
    /// </summary>
    public class ItemPartition
    {
        public PersistentItemType ItemType { get; set; }

        /// <summary>
        /// Position in data.
        /// </summary>
        public long Position { get; set; }

        /// <summary>
        /// Length of Partition.
        /// </summary>
        public long Size { get; set; }


        public long EndPosition
        {
            get { return this.Position + this.Size; }
        }
    }
}
