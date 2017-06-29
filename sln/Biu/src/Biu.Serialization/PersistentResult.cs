using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Serialization
{
    /// <summary>
    /// Result of a persistence operation.
    /// </summary>
    public class PersistentResult : IDisposable
    {
        public bool IsSucceeded { get; set; }

        public List<PersistentItem> Items { get; set; }


        public PersistentResult()
        {
            this.Items = new List<PersistentItem>();
        }

        ~PersistentResult()
        {
            Dispose(false);
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
                foreach (var item in this.Items)
                {
                    item.Dispose();
                }
                Items.Clear();
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
