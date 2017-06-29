using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;
using System.IO;
using System.Reflection;

namespace Biu.Diagnostics
{
    public class TextFileLogElement : LogElementBase
    {
        [ConfigurationProperty("dir", IsRequired = true)]
        public string Dir
        {
            get { return (string)this["dir"]; }
            set { this["dir"] = value; }
        }

        [ConfigurationProperty("maxLogFilesCount", DefaultValue = 365)]
        public int MaxLogFilesCount
        {
            get { return (int)this["maxLogFilesCount"]; }
            set { this["maxLogFilesCount"] = value; }
        }

        [ConfigurationProperty("absoluteDir", DefaultValue = false)]
        public bool AbsoluteDir
        {
            get { return (bool)this["absoluteDir"]; }
            set { this["absoluteDir"] = value; }
        }


        public TextFileLogElement()
        {
            
        }

        public TextFileLogElement(string name)
        {
            this.Name = name;
        }

        public TextFileLogElement(string name, string dir, int maxLogFilesCount)
        {
            this.Name = name;
            this.Dir = dir;            
            this.MaxLogFilesCount = maxLogFilesCount;
        }

        public override ILogRecorder MakeRecorder()
        {
            try
            {
                string strDir = this.Dir;
                if (!this.AbsoluteDir)
                {
                    string strPath = Assembly.GetEntryAssembly().Location;
                    strPath = strPath.Substring(0, strPath.LastIndexOf("\\") + 1);
                    strDir = Path.Combine(strPath, strDir);
                }

                Biu.IO.FileUtil.EnsureDirsAlongPath(strDir, false);

                TextFileLogRecorder recorder = new TextFileLogRecorder()
                {
                    Key = this.Name,
                    Dir = strDir, 
                    MaxLogFilesCount = this.MaxLogFilesCount,
                    RecordConverter = Activator.CreateInstance(this.Converter) as ILogRecordConverter
                };

                return recorder;
            }
            catch
            {
                throw;
            }
        }
    }
}
