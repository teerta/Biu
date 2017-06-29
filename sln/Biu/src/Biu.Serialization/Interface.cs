using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Serialization
{
    /// <summary>
    /// Serialize and Deserialize a PersistentItem
    /// </summary>
    /// <see cref="Biu.Serialization.PersistentItem"/>
    public interface IPersistentItemFormatter
    {
        string Name { get; set; }


        /// <summary>
        /// Serialize item to file.
        ///     item.Stream has data to be serialized
        /// </summary>
        /// <param name="item"></param>
        void Serialize(PersistentItem item);

        /// <summary>
        /// Put deserialized data into item.Stream.
        /// </summary>
        /// <param name="item"></param>
        /// <param name="offset"></param>
        /// <returns></returns>
        bool Deserialize(PersistentItem item, out long offset);
    }
}
