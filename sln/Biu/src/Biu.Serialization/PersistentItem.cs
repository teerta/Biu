using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Biu.Native.Security;

namespace Biu.Serialization
{
    /// <summary>
    /// The fundamental of persistence (for serializing and deserializing).
    /// </summary>
    /// <remarks>
    /// . PersistenceItem represent a File, Stream (part or whole) or binary data.
    /// . PersistenceItem can be encrypted (optional).
    /// . Property Order is for sorting when PersistenceItem represents part of something.
    /// </remarks>
    public class PersistentItem : IDisposable
    {        
        public PersistentItemType ItemType { get; set; }

        /// <summary>
        /// Determinate which Property has data (File, Stream and Binary).
        /// </summary>
        public PersistentItemStoredType StoredType { get; set; }

        /// <summary>
        /// Set this property to Important if PersistentItem is something critical of the whole data.
        /// </summary>
        public PersistentItemSignificance Significance { get; set; }

        public string File { get; set; }

        public Stream Stream { get; set; }

        public IEnumerable<Byte> Binary { get; set; }

        public bool IsEncrypted { get; set; }

        /// <summary>
        /// Mark serialization for PersistemceItem.
        /// </summary>
        public bool NeedSerialization { get; set; }

        public IEnumerable<byte> Hash { get; set; }

        /// <summary>
        /// Sort-order of thie PersistenceItem (part of whole data)
        /// </summary>
        public int Order { get; set; }


        public PersistentItem()
        {
            this.ItemType = PersistentItemType.Content;
            this.StoredType = PersistentItemStoredType.Stream;
            this.Significance = PersistentItemSignificance.Normal;
        }

        ~PersistentItem()
        {
            Dispose(false);
        }

        /// <summary>
        /// Create PersistenceItem's Stream (MemoryStream).
        /// (For PersistenceBase-Inherited class)
        /// </summary>
        public void CreateMemoryStream()
        {
            try
            {
                if (null != this.Stream)
                {
                    return;
                }

                this.Stream = new MemoryStream();
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Close PersistenceItem's Stream.
        /// (For PersistenceBase-Inherited class)
        /// </summary>
        public void CloseStream()
        {
            if (null != this.Stream)
            {
                this.Stream.Close();
                this.Stream = null;
            }
        }

        /// <summary>
        /// Correct PersistenceItem's Data
        /// . Copy Stream to Binary if StoredType is Binary.
        /// </summary>
        public void EnsureData()
        {
            if (null == this.Stream)
            {
                return;
            }

            switch (this.StoredType)
            {
                case PersistentItemStoredType.Binary:
                    var buff = new byte[this.Stream.Length];
                    this.Stream.Read(buff, 0, (int)this.Stream.Length);
                    this.Binary = buff;
                    break;
            }
        }

        /// <summary>
        /// Convert PersistenceItem.Stream to byte array.
        /// </summary>
        /// <returns></returns>
        public byte[] ToArray()
        {
            if (null == this.Stream)
            {
                return null;
            }

            var buff = new byte[this.Stream.Length];
            this.Stream.Seek(0L, SeekOrigin.Begin);
            this.Stream.Read(buff, 0, (int)this.Stream.Length);

            return buff;
        }

        #region IDisposable
        private bool disposed = false;

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                if (null != this.Stream)
                {
                    this.Stream.Close();
                    this.Stream = null;
                }
            }

            disposed = true;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }
        #endregion // IDisposable
    }
}
