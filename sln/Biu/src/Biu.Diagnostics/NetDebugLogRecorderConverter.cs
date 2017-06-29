using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Diagnostics.Tso;

namespace Biu.Diagnostics
{
    public class NetDebugLogRecorderConverter : ILogRecordConverter<Tso.NetLogRecord>
    {
        public NetLogRecord ConvertTo(LogRecord logRecord)
        {
            try
            {
                return new Tso.NetLogRecord()
                {
                    Level = logRecord.Level,
                    Date = DateTime.Now,
                    SiteName = logRecord.SiteName,
                    OperationName = logRecord.OperationName,
                    ErrorCode = logRecord.ErrorCode,
                    Remark = logRecord.Remark,
                    SuspiciousCause = logRecord.SuspiciousCause,
                    LogString = logRecord.LogString
                };
            }
            catch
            {
                throw;
            }
        }
    }
}
