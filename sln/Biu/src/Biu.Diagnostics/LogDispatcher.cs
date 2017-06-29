using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Concurrent;

namespace Biu.Diagnostics
{
    public class LogDispatcher : IDisposable
    {
        protected List<LogRecord> _records;
        protected object _syncRoot;
        protected ManualResetEvent _evtRecord;
        protected ManualResetEvent _evtDone;
        protected Thread _threadDispatcher;


        public LogDispatcher()
        {           
            try
            {
                _records = new List<LogRecord>();
                _syncRoot = new object();
                _evtRecord = new ManualResetEvent(false);
                _evtDone = new ManualResetEvent(false);
                _threadDispatcher = new Thread(new ThreadStart(ThreadDispatcher));
                _threadDispatcher.Start();
            }
            catch
            {
                throw;
            }
        }

        ~LogDispatcher()
        {
            Dispose(false);
        }

        public virtual void AddLog(LogRecord logRecord, bool dispatch = true)
        {
            if (null == logRecord)
            {
                return;
            }

            try
            {
                lock(_syncRoot)
                {
                    _records.Add(logRecord);
                }

                if (dispatch)
                {
                    _evtRecord.Set();
                }
            }
            catch
            {
                throw;
            }
        }

        public virtual void AddLog(IEnumerable<LogRecord> logRecords, bool dispatch = true)
        {
            if (null == logRecords || 0 != logRecords.Count())
            {
                return;
            }

            try
            {
                lock (_syncRoot)
                {
                    _records.AddRange(logRecords);
                }

                if (dispatch)
                {
                    _evtRecord.Set();
                }
            }
            catch
            {
                throw;
            }
        }

        protected void ThreadDispatcher()
        {
            try
            {
                LogManager.Instance.OpenRecorders(null);

                var waits = new WaitHandle[] { _evtDone, _evtRecord };

                int nWaitIndex = 0;
                var records = new List<LogRecord>();
                while (0 != (nWaitIndex = WaitHandle.WaitAny(waits)))
                {
                    _evtRecord.Reset();
                    lock(_syncRoot)
                    {
                        records.AddRange(_records);
                        _records.Clear();
                    }

                    records.ForEach(record =>
                    {
                        LogManager.Instance.Record(record);
                    });

                    records.Clear();
                }

                LogManager.Instance.CloseRecorders();
            }
            catch
            {
                throw;
            }
        }


        #region IDisposable
        private bool disposed = false;

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {

            }

            if (null != _threadDispatcher)
            {
                if (null != _evtDone)
                {
                    _evtDone.Set();
                }

                if (!_threadDispatcher.Join(5000))
                {
                    _threadDispatcher.Abort();
                }

                _threadDispatcher = null;

                if (null != _evtRecord)
                {
                    _evtRecord.Dispose();
                    _evtRecord = null;
                }

                if (null != _evtDone)
                {
                    _evtDone.Dispose();
                    _evtDone = null;
                }
            }            

            disposed = true;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }
        #endregion // IDisposable
    }
}
