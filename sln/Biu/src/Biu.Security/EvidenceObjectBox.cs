using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Concurrent;

namespace Biu.Security
{
    /// <summary>
    /// Storing object by generated key (evidence).
    /// Thread Safe
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class EvidenceObjectBox<T>
        where T : class
    {
        private ConcurrentDictionary<string, T> _objs = new ConcurrentDictionary<string, T>();

        private EvidenceObjectBox()
        {
        }

        public string Add(T obj)
        {
            try
            {
                string evidence = string.Empty;
                do
                {
                    string key1 = Guid.NewGuid().ToString();
                    string key2 = DateTime.UtcNow.ToString();
                    evidence = string.Format("{0}{1}", Hasher.HashTextUTF8(HashMethod.MD5, key1), Hasher.HashTextUTF8(HashMethod.SHA256, key2));
                } while (_objs.ContainsKey(evidence));

                _objs.AddOrUpdate(evidence, obj, (k, v) => { return v; });

                return evidence;
            }
            catch
            {
                throw;
            }
        }

        public T Remove(string evidence)
        {
            try
            {
                if (!_objs.ContainsKey(evidence))
                {
                    return null;
                }

                T obj;
                while (!_objs.TryRemove(evidence, out obj))
                {
                    System.Threading.Thread.Sleep(50);
                }

                return obj;
            }
            catch
            {
                throw;
            }
        }

        public T Get(string evidence)
        {
            try
            {
                if (!_objs.ContainsKey(evidence))
                {
                    return null;
                }

                T obj = null;
                while (!_objs.TryGetValue(evidence, out obj))
                {
                    System.Threading.Thread.Sleep(50);
                }

                return obj;
            }
            catch
            {
                throw;
            }
        }

        public void Clear()
        {            
            try
            {
                _objs.Clear();
            }
            catch
            {
                throw;
            }
        }
    }
}
