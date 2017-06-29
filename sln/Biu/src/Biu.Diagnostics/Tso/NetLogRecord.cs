using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;

namespace Biu.Diagnostics.Tso
{
    public class NetLogRecord : IBiuSerializable
    {
        public RecordLevel Level;

        public DateTime Date;

        public string SiteName;

        public string OperationName;

        public string SuspiciousCause;

        public string Remark;

        public long ErrorCode;

        public string LogString;


        public NetLogRecord()
        {

        }

        public virtual bool BSerialize(BiuSerializer ser)
        {
            bool bRet = true;
            try
            {
                ser
                    .Serialize((short)Level)
                    .Serialize(Date)
                    .Serialize(SiteName)
                    .Serialize(OperationName)
                    .Serialize(SuspiciousCause)
                    .Serialize(Remark)
                    .Serialize(ErrorCode)
                    .Serialize(LogString);
            }
            catch
            {
                bRet = false;
            }

            return bRet;
        }

        public virtual bool BDeserialize(BiuSerializer ser)
        {
            bool bRet = true;
            try
            {
                short nVal = 0;

                ser
                    .Deserialize(ref nVal)
                    .Deserialize(ref Date)
                    .Deserialize(ref SiteName)
                    .Deserialize(ref OperationName)
                    .Deserialize(ref SuspiciousCause)
                    .Deserialize(ref Remark)
                    .Deserialize(ref ErrorCode)
                    .Deserialize(ref LogString);

                Level = (RecordLevel)nVal;
            }
            catch
            {
                bRet = false;
            }

            return bRet;
        }

        public override int GetHashCode()
        {
            return this.ToString().GetHashCode();
        }

        public override string ToString()
        {
            try
            {
                var sb = new StringBuilder();
                sb
                    .AppendFormat("[{0}]", Date).Append(Environment.NewLine)
                    .AppendFormat("Level: {0}", Level.ToString()).Append(Environment.NewLine)
                    .AppendFormat("Site: {0}, Operation: {1}", SiteName, OperationName).Append(Environment.NewLine)
                    .AppendFormat("ErrorCode: {0}", ErrorCode).Append(Environment.NewLine);

                if (!string.IsNullOrEmpty(SuspiciousCause))
                {
                    sb.AppendFormat("SuspiciousCause: {0}", SuspiciousCause).Append(Environment.NewLine);
                }

                if (!string.IsNullOrEmpty(Remark))
                {
                    sb.AppendFormat("Remark: {0}", Remark).Append(Environment.NewLine);
                }

                if (!string.IsNullOrEmpty(LogString))
                {
                    sb.Append(LogString);
                }                

                return sb.ToString();
            }
            catch
            {
                throw;
            }
        }
    }
}
