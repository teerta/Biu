using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using System.Threading;

namespace Biu
{
    public class ClassifiedBranch
    {
        private ConcurrentDictionary<ulong, ClassifiedObject> _indexObjects;


        public long ID { get; set; }

        public string Class { get; set; }

        public string Name { get; set; }

        public object Tag { get; set; }


        public ConcurrentDictionary<string, ClassifiedObject> Objects { get; private set; }


        public ClassifiedBranch()
        {
            _indexObjects = new ConcurrentDictionary<ulong, ClassifiedObject>();

            this.Objects = new ConcurrentDictionary<string, ClassifiedObject>();

            this.ID = 0;
            this.Class = string.Empty;
            this.Name = string.Empty;
        }

        public string Add<T>(T val, long oid = 0, string name = null, object tag = null, ulong indexKey = 0ul)
        {
            try
            {
                var obj = ClassifiedObject.FromOID(oid, name, tag);
                obj.IndexKey = indexKey;
                obj.Value = val;
                string strKey = obj.ToString();
                this.Objects.AddOrUpdate(strKey, obj, (k, v) => { return obj; });
                if (0ul != indexKey)
                {
                    _indexObjects.AddOrUpdate(indexKey, obj, (k, v) => { return obj; });
                }

                return strKey;
            }
            catch
            {
                throw;
            }
        }

        public string AddWithUser<T>(T val, long oid = 0, string name = null, long uid = 0, string userName = null, long userSesionID = 0, 
            object tag = null, ulong indexKey = 0ul)
        {
            try
            {
                var obj = ClassifiedObject.FromUser(oid, name, uid, userName, userSesionID, tag);
                obj.IndexKey = indexKey;
                obj.Value = val;
                string strKey = obj.ToString();
                this.Objects.AddOrUpdate(strKey, obj, (k, v) => { return obj; });
                if (0ul != indexKey)
                {
                    _indexObjects.AddOrUpdate(indexKey, obj, (k, v) => { return obj; });
                }

                return strKey;
            }
            catch
            {
                throw;
            }
        }

        public T Get<T>(long oid = 0, string name = null)
            where T : class, new()
        {
            try
            {
                var obj = GetObject(ClassifiedObject.MakeKey(oid, name));
                if (null == obj)
                {
                    return null;
                }

                return obj.GetValue<T>();
            }
            catch
            {
                throw;
            }
        }

        public T GetWithUser<T>(long oid = 0, string name = null, long uid = 0, string userName = null, long userSesionID = 0)
            where T : class, new()
        {
            try
            {
                var obj = GetObject(ClassifiedObject.MakeKeyWithUser(oid, name, uid, userName, userSesionID));
                if (null == obj)
                {
                    return null;
                }

                return obj.GetValue<T>();
            }
            catch
            {
                throw;
            }
        }

        public T GetByIndexKey<T>(ulong indexKey)
            where T : class, new()
        {
            try
            {
                ClassifiedObject obj;
                if (_indexObjects.TryGetValue(indexKey, out obj))
                {
                    obj.GetValue<T>();
                }

                return null;
            }
            catch
            {
                throw;
            }
        }

        public Tuple<T, object> TagGet<T>(long oid = 0, string name = null)
            where T : class, new()
        {
            try
            {
                var obj = GetObject(ClassifiedObject.MakeKey(oid, name));
                if (null == obj)
                {
                    return null;
                }

                return Tuple.Create(obj.GetValue<T>(), obj.Tag);
            }
            catch
            {
                throw;
            }
        }

        public Tuple<T, object> TagGetWithUser<T>(long oid = 0, string name = null, long uid = 0, string userName = null, long userSesionID = 0)
            where T : class, new()
        {
            try
            {
                var obj = GetObject(ClassifiedObject.MakeKeyWithUser(oid, name, uid, userName, userSesionID));
                if (null == obj)
                {
                    return null;
                }

                return Tuple.Create(obj.GetValue<T>(), obj.Tag);
            }
            catch
            {
                throw;
            }
        }

        public Tuple<T, object> TagGetByIndexKey<T>(ulong indexKey)
            where T : class, new()
        {
            try
            {                
                ClassifiedObject obj;
                if (_indexObjects.TryGetValue(indexKey, out obj))
                {
                    Tuple.Create(obj.GetValue<T>(), obj.Tag);
                }

                return null;
            }
            catch
            {
                throw;
            }
        }

        public void Remove(long oid = 0, string name = null)
        {
            try
            {
                RemoveObject(ClassifiedObject.MakeKey(oid, name));
            }
            catch
            {
                throw;
            }
        }

        public void RemoveWithUser(long oid = 0, string name = null, long uid = 0, string userName = null, long userSesionID = 0)
        {
            try
            {
                RemoveObject(ClassifiedObject.MakeKeyWithUser(oid, name, uid, userName, userSesionID));
            }
            catch
            {
                throw;
            }
        }

        public void RemoveByIndexKey(ulong indexKey)
        {
            try
            {                
                ClassifiedObject obj;
                if (_indexObjects.TryRemove(indexKey, out obj))
                {
                    obj.IndexKey = 0ul;
                    RemoveObject(obj.ToString());
                }                
            }
            catch
            {
                throw;
            }
        }

        public void ForEachObject(Action<ClassifiedObject> action)
        {
            try
            {
                foreach (var pair in this.Objects)
                {
                    action(pair.Value);
                }
            }
            catch
            {
                throw;
            }
        }

        private ClassifiedObject GetObject(ClassifiedObject obj)
        {
            try
            {
                string strKey = obj.ToString();
                ClassifiedObject ret;
                if (this.Objects.TryGetValue(strKey, out ret))
                {
                    return ret;
                }

                return null;
            }
            catch
            {
                throw;
            }
        }

        private ClassifiedObject GetObject(string key)
        {
            try
            {                
                ClassifiedObject ret;
                if (this.Objects.TryGetValue(key, out ret))
                {
                    return ret;
                }

                return null;
            }
            catch
            {
                throw;
            }
        }

        private ClassifiedObject RemoveObject(string key)
        {
            try
            {
                ClassifiedObject obj = null;
                if  (this.Objects.TryRemove(key, out obj))
                {
                    if (0ul != obj.IndexKey)
                    {
                        ulong ulIndexKey = obj.IndexKey;
                        _indexObjects.TryRemove(ulIndexKey, out obj);
                    }
                }
               
                return obj;
            }
            catch
            {
                throw;
            }
        }

        public override string ToString()
        {
            return string.Format("{0}{1}{2}", Class, ID, Name);
        }

        public override int GetHashCode()
        {
            return this.ToString().GetHashCode();
        }
    }
}
