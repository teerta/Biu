using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;
using System.ComponentModel;

namespace Biu.Diagnostics
{
    public abstract class LogElementBase : ConfigurationElement
    {
        [ConfigurationProperty("name", DefaultValue = null, IsKey = true, IsRequired = true)]
        public string Name
        {
            get { return (string)this["name"]; }
            set { this["name"] = value; }
        }

        [ConfigurationProperty("enabled", DefaultValue = true)]
        public bool Enabled
        {
            get { return (bool)this["enabled"]; }
            set { this["enabled"] = value; }
        }

        [ConfigurationProperty("converter", DefaultValue = typeof(TextRecorderConverter))]
        [TypeConverter(typeof(TypeNameConverter))]
        public Type Converter
        {
            get { return (Type)this["converter"]; }
            set { this["converter"] = value; }
        }

        public abstract ILogRecorder MakeRecorder();
    }
}
