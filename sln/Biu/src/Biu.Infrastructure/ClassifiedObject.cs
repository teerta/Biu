using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu
{
    public class ClassifiedObject
    {
        public bool HasUserIdentity { get; set; }

        public long OID { get; set; }

        public string Name { get; set; }

        public long UID { get; set; }

        public string UserName { get; set; }

        public long UserSessionID { get; set; }

        public object Tag { get; set; }

        public object Value { get; set; }

        public ulong IndexKey { get; set; }


        public static ClassifiedObject FromOID(long oid = 0, string name = null, object tag = null)
        {
            if (null == name)
            {
                name = string.Empty;
            }

            var obj = new ClassifiedObject()
            {
                OID = oid,
                Name = name,
                Tag = tag
            };

            return obj;
        }

        public static ClassifiedObject FromUser(long oid = 0, string name = null, long uid = 0, string userName = null, long userSesionID = 0, object tag = null)
        {
            if (null == name)
            {
                name = string.Empty;
            }

            if (null == userName)
            {
                userName = string.Empty;
            }

            var obj = new ClassifiedObject()
            {
                HasUserIdentity = true,
                OID = oid,
                Name = name,
                UID = uid,
                UserName = userName,
                UserSessionID = userSesionID,
                Tag = tag
            };

            return obj;
        }


        public ClassifiedObject()
        {
            this.OID = 0;
            this.Name = string.Empty;
            this.UID = 0;
            this.UserName = string.Empty;
            this.UserSessionID = 0;
        }

        public T GetValue<T>()
            where T : class
        {
            return this.Value as T;
        }

        public override string ToString()
        {
            var str = string.Format("{0}{1}", this.OID, this.Name);
            if (this.HasUserIdentity)
            {
                str = string.Format("{0}{1}{2}{3}", str, UID, UserName, UserSessionID);
            }

            return str;
        }

        public override int GetHashCode()
        {
            return this.ToString().GetHashCode();
        }

        public static string MakeKey(long oid = 0, string name = null)
        {
            if (null == name)
            {
                name = string.Empty;
            }

            return string.Format("{0}{1}", oid, name);
        }

        public static string MakeKeyWithUser(long oid = 0, string name = null, long uid = 0, string userName = null, long userSesionID = 0)
        {
            if (null == name)
            {
                name = string.Empty;
            }

            if (null == userName)
            {
                userName = string.Empty;
            }

            return string.Format("{0}{1}{2}{3}", MakeKey(oid, name), uid, userName, userSesionID);
        }
    }
}
