using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;
using System.Reflection;

namespace Biu.Serialization
{
    /// <summary>
    /// To bind to correct class when u load multiple Dlls that have the same class (same namespaces).
    /// </summary>
    public class UniqueClassBinder : SerializationBinder
    {
        public override Type BindToType(string assemblyName, string typeName)
        {
            Type tType = null;
            string strShortAssemblyName = assemblyName.Split(',')[0];
            Assembly[] ayAssemblies = AppDomain.CurrentDomain.GetAssemblies();
            foreach (Assembly ayAssembly in ayAssemblies)
            {
                if (strShortAssemblyName == ayAssembly.FullName.Split(',')[0])
                {
                    tType = ayAssembly.GetType(typeName);
                    break;

                }
            }

            return tType;
        }
    }
}
