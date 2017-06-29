using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Native.Security;

namespace Biu.Serialization
{
    /// <summary>
    /// Determinate the formatter, stored-type and etc. for an ItemPartition
    /// PersistenceBase and its subclasses do serialization and deserialization with PersistenceDecision.
    /// </summary>
    public class PersistenceDecision
    {
        /// <summary>
        /// Key for encryption and decryption.
        /// Null means no-encrypt.
        /// </summary>
        public CryptKey Key { get; set; }

        /// <summary>
        /// Formatter for serialization.
        /// Null means no-format-operation.
        /// </summary>
        public IPersistentItemFormatter Formatter { get; set; }

        public PersistentItemStoredType StoredType { get; set; }

        public PersistentItemSignificance Significance { get; set; }

        public bool CloseStreamWhenDone { get; set; }


        public PersistenceDecision()
        {
            this.StoredType = PersistentItemStoredType.Stream;
            this.Significance = PersistentItemSignificance.Normal;
        }
    }
}
