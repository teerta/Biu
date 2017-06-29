using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;

namespace Biu.Util
{
    public static class ConfigHelper
    {
        public static T GetAppSetting<T>(string key, T defaultVal, ParseValue<T> parse)
            where T : struct
        {            
            if (ConfigurationManager.AppSettings.AllKeys.Contains(key))
            {
                T val;
                if (parse(ConfigurationManager.AppSettings[key], out val))
                {
                    return val;
                }
            }

            return defaultVal;
        }

        public static string GetAppSetting(string key, string defaultVal, bool trim = true)
        {
            if (ConfigurationManager.AppSettings.AllKeys.Contains(key))
            {
                return trim ? ConfigurationManager.AppSettings[key].Trim() : ConfigurationManager.AppSettings[key];
            }

            return defaultVal;
        }
    }
}
