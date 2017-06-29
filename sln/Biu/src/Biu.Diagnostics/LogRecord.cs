using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Diagnostics
{
    public class LogRecord
    {
        public RecordLevel Level { get; set; } = RecordLevel.Message;

        public string SiteName { get; set; }

        public string OperationName { get; set; }

        public string SuspiciousCause { get; set; }

        public string Remark { get; set; }

        public long ErrorCode { get; set; }

        public Exception TargetException { get; set; }

        public object Tag { get; set; }


        public bool HasSuspicioisCause
        {
            get { return !string.IsNullOrEmpty(this.SuspiciousCause); }
        }

        public bool HasRemark
        {
            get { return !string.IsNullOrEmpty(this.Remark); }
        }

        public string VerboseLogString
        {
            get
            {
                if (null == this.TargetException)
                {
                    return string.Empty;
                }

                var sb = new StringBuilder();
                var exception = this.TargetException;
                sb.Append("Exception:").Append(Environment.NewLine);
                while (null != exception)
                {
                    sb
                        .Append("@---------").Append(Environment.NewLine)
                        .Append("Message:").Append(Environment.NewLine).Append(exception.Message).Append(Environment.NewLine);

                    if (!string.IsNullOrEmpty(exception.Source))
                    {
                        sb.AppendFormat("Source: {0}", exception.Source).Append(Environment.NewLine);
                    }

                    if (0 != exception.HResult)
                    {
                        sb.AppendFormat("HResult: {0}", exception.HResult).Append(Environment.NewLine);
                    }

                    if (!string.IsNullOrEmpty(exception.StackTrace))
                    {
                        sb.Append("Stack Trace:").Append(Environment.NewLine)
                            .Append(exception.StackTrace).Append(Environment.NewLine);
                    }

                    if (null != exception.TargetSite)
                    {
                        sb.Append("Target Site:").Append(Environment.NewLine)
                            .Append(exception.TargetSite.ToString()).Append(Environment.NewLine);
                    }

                    if (!string.IsNullOrEmpty(exception.HelpLink))
                    {
                        sb.Append("Help Link:").Append(Environment.NewLine)
                            .Append(exception.HelpLink).Append(Environment.NewLine);
                    }

                    if (0 != exception.Data.Count)
                    {
                        sb.Append("Data:").Append(Environment.NewLine);
                        foreach (System.Collections.DictionaryEntry pair in exception.Data)
                        {
                            sb.AppendFormat("Key: {0} <-> Value {1}", pair.Key, pair.Value);
                        }
                        sb.Append(Environment.NewLine);
                    }

                    sb.Append("----------");

                    exception = exception.InnerException;
                }

                return sb.ToString();
            }

        }

        public string LogString
        {
            get
            {
                if (null == this.TargetException)
                {
                    return string.Empty;
                }

                var sb = new StringBuilder();
                var exception = this.TargetException;
                sb.Append("Exception:").Append(Environment.NewLine);
                while (null != exception)
                {
                    sb
                        .Append("@---------").Append(Environment.NewLine)
                        .Append(exception.ToString()).Append(Environment.NewLine)
                        .Append("----------");

                    exception = exception.InnerException;
                }

                return sb.ToString();
            }
        }
    }
}
