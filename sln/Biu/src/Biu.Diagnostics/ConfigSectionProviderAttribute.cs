﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Diagnostics
{
    [AttributeUsage(AttributeTargets.Class, Inherited = false, AllowMultiple = false)]
    public sealed class ConfigSectionProviderAttribute : Attribute
    {
        public ConfigSectionProviderAttribute()
        {

        }
    }
}
