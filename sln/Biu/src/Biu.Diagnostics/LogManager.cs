using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Configuration;

namespace Biu.Diagnostics
{
    public sealed class LogManager
    {
        #region Singleton
        private static readonly Lazy<LogManager> _lazyInstance =
            new Lazy<LogManager>(() => { return new LogManager(); });

        public static LogManager Instance { get { return _lazyInstance.Value; } }
        #endregion //Singleton


        //private bool _hasLogRecorders; 


        //public string ProvidersDir { get; private set; }

        public bool IsEnabled { get; set; } = true;        

        public LogDispatcher Dispatcher { get; set; }


        public List<ILogRecorder> Recorders = new List<ILogRecorder>();


        private LogManager()
        {
        }

        //public bool LoadProviders()
        //{
        //    try
        //    {
        //        if (!Directory.Exists(this.ProvidersDir))
        //        {
        //            return false;
        //        }

        //        var configuration = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
        //        var typeSection = typeof(ConfigurationSection);
        //        foreach (var provider in Directory.GetFiles(this.ProvidersDir, "*.dll", SearchOption.TopDirectoryOnly))
        //        {
        //            var assembly = Assembly.LoadFrom(provider);
        //            foreach (var exportedType in assembly.GetExportedTypes())
        //            {
        //                var attr = exportedType.GetCustomAttribute<ConfigSectionProviderAttribute>();
        //                if (null == attr)
        //                {
        //                    continue;
        //                }

        //                if (!exportedType.IsSubclassOf(typeSection))
        //                {
        //                    continue;
        //                }

        //                configuration.Sections.Add(exportedType.Name, Activator.CreateInstance(exportedType) as ConfigurationSection);
        //            }
        //        }

        //        if (0 != this.Recorders.Count)
        //        {
        //            _hasLogRecorders = true;
        //            return true;
        //        }

        //        return false;
        //    }
        //    catch
        //    {
        //        throw;
        //    }
        //}

        public bool LoadRecorders()
        {
            try
            {
                var biuLogSection = ConfigurationManager.GetSection("biuLog") as BiuLog;
                if (null == biuLogSection)
                {
                    return false;
                }

                this.IsEnabled = biuLogSection.Enabled;

                bool bRet = false;
                if (!string.IsNullOrEmpty(biuLogSection.TextLog.Name) && biuLogSection.TextLog.Enabled)
                {
                    this.Recorders.Add(biuLogSection.TextLog.MakeRecorder());
                }

                if (!string.IsNullOrEmpty(biuLogSection.NetLog.Name) && biuLogSection.NetLog.Enabled)
                {
                    this.Recorders.Add(biuLogSection.NetLog.MakeRecorder());
                }

                return bRet;
            }
            catch
            {
                throw;
            }
        }

        public bool InitializeRecorders<T>()
            where T : LogDispatcher, new()
        {
            //if (!_hasLogRecorders)
            //{
            //    return false;
            //}

            try
            {               
                bool bRet = false;
                this.Recorders.ForEach(recorder =>
                {
                    if (recorder.Initialize())
                    {
                        bRet = true;
                    }
                    else
                    {
                        recorder.IsAvailable = false;
                    }
                });

                this.Dispatcher = new T();

                return bRet;
            }
            catch
            {
                throw;
            }
        }

        public void UninitializeRecorders()
        {
            //if (!_hasLogRecorders)
            //{
            //    return;
            //}

            try
            {
                if (null != this.Dispatcher)
                {
                    this.Dispatcher.Dispose();
                    this.Dispatcher = null;
                }

                this.Recorders.ForEach(recorder => recorder.Uninitialize());
            }
            catch
            {
                throw;
            }
        }

        public bool OpenRecorders(object arg)
        {
            //if (!_hasLogRecorders)
            //{
            //    return false;
            //}

            try
            {
                bool bRet = false;
                this.Recorders.ForEach(recorder =>
                {
                    if (recorder.Open(arg))
                    {
                        bRet = true;
                    }
                    else
                    {
                        recorder.IsAvailable = false;
                    }
                });

                return bRet;
            }
            catch
            {
                throw;
            }
        }

        public void CloseRecorders()
        {
            //if (!_hasLogRecorders)
            //{
            //    return;
            //}

            try
            {
                this.Recorders.ForEach(recorder => recorder.Close());
            }
            catch
            {
                throw;
            }
        }

        public void Record(LogRecord logRecord)
        {
            if (/*!_hasLogRecorders || */null == logRecord)
            {
                return;
            }                       

            try
            {
                this.Recorders.ForEach(recorder =>
                {
                    if (recorder.IsAvailable)
                    {
                        recorder.Record(logRecord);
                    }
                });                
            }
            catch
            {
                throw;
            }            
        }

        public void Log(LogRecord logRecord, bool write = true)
        {
            if (null == this.Dispatcher)
            {
                return;
            }

            if (!this.IsEnabled)
            {
                if (null != logRecord.TargetException)
                {
                    throw logRecord.TargetException;
                }

                return;
            }

            try
            {
                this.Dispatcher.AddLog(logRecord, write);
            }
            catch
            {
                throw;
            }
        }

        [Obsolete]
        public void Log(IEnumerable<LogRecord> logRecords, bool write = true)
        {
            if (null == this.Dispatcher)
            {
                return;
            }

            try
            {
                this.Dispatcher.AddLog(logRecords, write);
            }
            catch
            {
                throw;
            }
        }
    }
}
