using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;

namespace Biu.Diagnostics
{
    public class BiuLog : ConfigurationSection
    {
        [ConfigurationProperty("textLog")]
        public TextFileLogElement TextLog
        {
            get { return (TextFileLogElement)base["textLog"]; }
        }

        [ConfigurationProperty("netLog")]
        public NetDebugLogElement NetLog
        {
            get { return (NetDebugLogElement)base["netLog"]; }
        }

        [ConfigurationProperty("enabled", DefaultValue = true)]
        public bool Enabled
        {
            get { return (bool)this["enabled"]; }
            set { this["enabled"] = value; }
        }
    }
}
