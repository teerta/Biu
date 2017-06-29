using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Diagnostics
{
    public class TextRecorderConverter : ILogRecordConverter<string>
    {
        public string ConvertTo(LogRecord logRecord)
        {
            try
            {
                var sb = new StringBuilder();
                sb
                    .AppendFormat("Level: {0}", logRecord.Level.ToString()).Append(Environment.NewLine)
                    .AppendFormat("Site: {0}, Operation: {1}", logRecord.SiteName, logRecord.OperationName).Append(Environment.NewLine)
                    .AppendFormat("ErrorCode: {0}", logRecord.ErrorCode).Append(Environment.NewLine);

                if (logRecord.HasSuspicioisCause)
                {
                    sb.AppendFormat("SuspiciousCause: {0}", logRecord.SuspiciousCause).Append(Environment.NewLine);
                }

                if (logRecord.HasRemark)
                {
                    sb.AppendFormat("Remark: {0}", logRecord.Remark).Append(Environment.NewLine);
                }

                sb.Append(logRecord.LogString);

                return sb.ToString();
            }
            catch
            {
                throw;
            }
        }
    }
}
