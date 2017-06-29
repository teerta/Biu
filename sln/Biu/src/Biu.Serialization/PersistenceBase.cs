using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Biu.Serialization
{
    /// <summary>
    /// Base class for persistence method.
    /// </summary>
    public abstract class PersistenceBase
    {
        /// <summary>
        /// Save a file.
        /// </summary>
        /// <param name="file"></param>
        /// <param name="nameItemFile">An optional method for naming each PersistentItem.</param>
        /// <returns></returns>
        public PersistentResult Save(string file, Func<PersistentItem, string> nameItemFile = null)
        {
            Stream stream = null;
            try
            {
                stream = new FileStream(file, FileMode.Open, FileAccess.ReadWrite, FileShare.Write);

                return Save(stream, nameItemFile);
            }
            catch
            {
                throw;
            }
            finally
            {
                if (null != stream)
                {
                    stream.Close();
                    stream = null;
                }
            }
        }

        /// <summary>
        /// Save a stream.
        /// </summary>
        /// <param name="stream">Stream to be saved.</param>
        /// <param name="nameItemFile">An optional method for naming each PersistentItem.</param>
        /// <returns></returns>
        public abstract PersistentResult Save(Stream stream, Func<PersistentItem, string> nameItemFile = null);

        /// <summary>
        /// Load from PersistenceItems.
        /// </summary>
        /// <param name="items"></param>
        /// <returns></returns>
        public abstract Stream Load(IEnumerable<PersistentItem> items);
    }
}
