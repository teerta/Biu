using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Biu.Diagnostics
{
    public sealed class TextFileLogRecorder : ILogRecorder
    {
        private DateTime _dtLast = DateTime.MinValue;
        private StreamWriter _sw;


        public string Key { get; set; }

        public string Dir { get; set; }

        public int MaxLogFilesCount { get; set; }


        #region ILogRecorder

        private bool _isAvailable = true;
        public bool IsAvailable
        {
            get
            {
                return _isAvailable && null != _sw;
            }

            set
            {
                _isAvailable = value;
            }
        }

        public ILogRecordConverter RecordConverter { get; set; }

        public void Close()
        {
            try
            {
                if (null != _sw)
                {
                    _sw.Close();
                    _sw = null;
                }
            }
            catch
            {
                throw;
            }
        }

        public bool Initialize()
        {
            return true;
        }

        public bool Open(object arg)
        {
            try
            {
                if (string.IsNullOrEmpty(this.Key) || null == this.RecordConverter || 
                    !Directory.Exists(this.Dir) || this.MaxLogFilesCount < 1)
                {
                    return false;
                }

                EnsureLogFile();

                return true;
            }
            catch
            {
                throw;
            }
        }

        public void Record(LogRecord logRecord)
        {
            if (null == logRecord)
            {
                return;
            }

            var converter = this.RecordConverter as ILogRecordConverter<string>;
            if (null == converter)
            {
                return;
            }

            try
            {
                EnsureLogFile();
                var now = DateTime.Now;
                _sw.WriteLine(string.Format("[{0}]", now.ToString()));
                _sw.WriteLine(converter.ConvertTo(logRecord));
                _sw.Flush();
            }
            catch
            {
                throw;
            }
        }

        public void Uninitialize()
        {
            
        }

        #endregion // ILogRecorder


        private void EnsureLogFile()
        {
            try
            {
                if (null != _sw)
                {
                    var now = DateTime.Now;
                    if ((now - _dtLast).Days != 0)
                    {
                        _sw.Close();
                        _sw = null;
                    }
                }

                if (null == _sw)
                {
                    string strFile = NextLogFileName();
                    bool bAppend = File.Exists(strFile);
                    if (!bAppend)
                    {
                        ClearLogFiles();
                    }

                    _sw = new StreamWriter(strFile, bAppend);
                }
            }
            catch
            {
                throw;
            }
        }

        private string NextLogFileName()
        {
            var now = DateTime.Now;

            return string.Format("{0}\\{1}-{2}.log", this.Dir, this.Key, now.ToString("yyyyMMdd"));
        }

        private void ClearLogFiles()
        {
            try
            {
                var logFiles = Directory.GetFiles(this.Dir, "*.log", SearchOption.TopDirectoryOnly);
                if (logFiles.Length < this.MaxLogFilesCount)
                {
                    return;
                }

                var orderedLogFiles = logFiles.OrderByDescending(log => File.GetCreationTime(log).Ticks);
                File.Delete(orderedLogFiles.LastOrDefault());
            }
            catch
            {
                throw;
            }
        }
    }
}
