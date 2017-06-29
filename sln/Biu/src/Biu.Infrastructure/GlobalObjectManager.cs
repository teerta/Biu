using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu
{
    /// <summary>
    /// A singleton class to manage named or typed class instance that is shared across application.
    /// </summary>
    public class GlobalObjectManager
    {
        #region Singleton
        private static readonly Lazy<GlobalObjectManager> _lazyInstance = 
            new Lazy<GlobalObjectManager>(() => { return new GlobalObjectManager(); });

        public static GlobalObjectManager Instance { get { return _lazyInstance.Value; } }
        #endregion //Singleton


        private GlobalObjectManager()
        {
            _dictTyped = new Dictionary<Type, object>();
            _dictNamed = new Dictionary<string, object>();
        }

        public T RegisterTypedObject<T>(T obj)
            where T : class, new()
        {
            if (null == obj)
            {
                throw new NullReferenceException("obj is null");
            }

            try
            {
                _dictTyped[typeof(object)] = obj;
                return obj;
            }
            catch
            {
                throw;
            }
        }

        public T GetTypedObject<T>(Type t)
            where T : class, new()
        {
            if (!_dictTyped.ContainsKey(t))
            {
                return default(T);
            }

            return _dictTyped[t] as T;
        }

        public T RemoveTypedObject<T>(Type t)
            where T : class, new()
        {
            if (!_dictTyped.ContainsKey(t))
            {
                return default(T);
            }

            var obj = _dictTyped[t];
            _dictTyped.Remove(t);

            return obj as T;
        }

        public T RegisterNamedObject<T>(string name, T obj)
            where T : class, new()
        {
            if (null == obj)
            {
                throw new NullReferenceException("obj is null");
            }

            try
            {
                _dictNamed[name] = obj;
                return obj;
            }
            catch
            {
                throw;
            }
        }

        public T GetNamedObject<T>(string name)
            where T : class, new()
        {
            if (!_dictNamed.ContainsKey(name))
            {
                return default(T);
            }

            return _dictNamed[name] as T;
        }

        public T RemoveNamedObject<T>(string name)
            where T : class, new()
        {
            if (!_dictNamed.ContainsKey(name))
            {
                return default(T);
            }

            var obj = _dictNamed[name];
            _dictNamed.Remove(name);

            return obj as T;
        }

        private Dictionary<Type, object> _dictTyped;

        private Dictionary<string, object> _dictNamed;
    }
}
