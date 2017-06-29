using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Transmission.Management
{
    public class PengingObjectManager
    {
        #region Singleton
        private static readonly Lazy<PengingObjectManager> _lazyInstance = 
            new Lazy<PengingObjectManager>(() => { return new PengingObjectManager(); });

        public static PengingObjectManager Instance { get { return _lazyInstance.Value; } }
        #endregion //Singleton

        private PengingObjectManager()
        {
        }

        private object _syncDictObj = new object();
        private Dictionary<long, object> _dictObj = new Dictionary<long, object>();

        public long Add(object obj)
        {
            if (null == obj)
            {
                return 0;
            }

            long lKey = 0;
            Random rand = new Random();
            lock (_syncDictObj)
            {
                lKey = DateTime.Now.ToBinary() + (DateTime.Now.ToBinary() ^ rand.Next());
                while (_dictObj.ContainsKey(lKey))
                {
                    lKey = DateTime.Now.ToBinary() + (DateTime.Now.ToBinary() ^ rand.Next());
                }

                _dictObj[lKey] = obj;
            }

            return lKey;
        }

        public T Get<T>(long lKey)
            where T : class
        {
            T obj = null;
            lock (_syncDictObj)
            {
                if (!_dictObj.ContainsKey(lKey))
                {
                    return null;
                }

                obj = _dictObj[lKey] as T;
            }

            return obj;
        }

        public T Retrive<T>(long lKey)
            where T : class
        {
            T obj = null;
            lock (_syncDictObj)
            {
                if (!_dictObj.ContainsKey(lKey))
                {
                    return null;
                }

                obj = _dictObj[lKey] as T;
                _dictObj.Remove(lKey);
            }

            return obj;
        }

        public void Remove(long lKey)
        {
            lock (_syncDictObj)
            {
                if (_dictObj.ContainsKey(lKey))
                {
                    _dictObj.Remove(lKey);
                }
            }
        }
    }
}
