using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

namespace Biu.Serialization
{
    /// <summary>
    /// A auxiliary class for serialization.
    /// </summary>
    public class SerializationUtil
    {
        /// <summary>
        /// Serialize object to file by binder.
        /// </summary>
        /// <typeparam name="F">IFormatter-implemented class</typeparam>
        /// <param name="path"></param>
        /// <param name="obj"></param>
        /// <param name="binder"></param>
        public static void SerializeToFile<F>(string path, object obj, SerializationBinder binder = null)
            where F : IFormatter, new()
        {
            try
            {
                F ser = new F();
                if (null != binder)
                {
                    ser.Binder = binder;
                }

                using (FileStream fs = File.Open(path, FileMode.Create, FileAccess.ReadWrite, FileShare.Read))
                {
                    ser.Serialize(fs, obj);
                }
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Deserialize object from file by binder.
        /// </summary>
        /// <typeparam name="F">IFormatter-implemented class</typeparam>
        /// <typeparam name="T">Type of deserialized object</typeparam>
        /// <param name="path"></param>
        /// <param name="binder"></param>
        /// <returns></returns>
        public static T DeserializeFromFile<F, T>(string path, SerializationBinder binder = null)
            where F : IFormatter, new()
            where T : class
        {
            try
            {
                if (!File.Exists(path))
                {
                    return null;
                }

                F ser = new F();
                if (null != binder)
                {
                    ser.Binder = binder;
                }
                T obj = null;
                using (FileStream fs = File.Open(path, FileMode.Open, FileAccess.Read, FileShare.Read))
                {
                    fs.Seek(0, SeekOrigin.Begin);
                    obj = ser.Deserialize(fs) as T;
                }

                return obj;
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Deserialize by UniqueClassBinder;
        /// </summary>
        /// <typeparam name="F">IFormatter-implemented class</typeparam>
        /// <typeparam name="T">Type of deserialized object</typeparam>
        /// <param name="path"></param>
        /// <returns></returns>
        /// <see cref="Biu.Serialization.UniqueClassBinder"/>
        public static T DeserializeFromFileUnique<F, T>(string path)
            where F : IFormatter, new()
            where T : class
        {
            try
            {
                return DeserializeFromFile<F, T>(path, new UniqueClassBinder());
            }
            catch
            {
                throw;
            }
        }
    }
}
