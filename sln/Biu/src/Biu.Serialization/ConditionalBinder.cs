using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;
using System.Reflection;

namespace Biu.Serialization
{
    public class ConditionalBinder : SerializationBinder
    {
        protected List<BinderReplacedItem> _replacedItems = new List<BinderReplacedItem>();

        public void AddReplaced(BinderReplacedItem item)
        {
            _replacedItems.Add(item);
        }

        public override Type BindToType(string assemblyName, string typeName)
        {
            bool bReplaced = false;
            Type tType = null;
            string strShortAssemblyName = assemblyName.Split(',')[0];
            Assembly[] ayAssemblies = AppDomain.CurrentDomain.GetAssemblies();
            foreach (var item in _replacedItems)
            {
                if (item.IsMatch(assemblyName, typeName))
                {
                    bReplaced = true;
                    tType = item.ToType;
                    break;
                }
            }

            if (bReplaced)
            {
                return tType;
            }

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
