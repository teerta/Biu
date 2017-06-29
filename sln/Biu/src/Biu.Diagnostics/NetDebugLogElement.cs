using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Configuration;
using System.Net;

namespace Biu.Diagnostics
{
    public class NetDebugLogElement : LogElementBase
    {
        [ConfigurationProperty("address", DefaultValue = "Any")]
        public string Address
        {
            get { return (string)this["address"]; }
            set { this["address"] = value; }
        }

        [ConfigurationProperty("port", IsRequired = true, DefaultValue = 9191)]
        public int Port
        {
            get { return (int)this["port"]; }
            set { this["port"] = value; }
        }


        public NetDebugLogElement()
        {

        }

        public NetDebugLogElement(string address, int port)
        {
            this.Address = address;
            this.Port = port;
        }

        public override ILogRecorder MakeRecorder()
        {
            try
            {
                IPAddress address = null;
                if (this.Address.ToLower() == "any")
                {
                    address = IPAddress.Any;
                }
                else
                {
                    address = IPAddress.Parse(this.Address);
                }

                return new NetDebugLogRecorder()
                {
                    DebugN1EndPoint = new IPEndPoint(address, this.Port),
                    RecordConverter = Activator.CreateInstance(this.Converter) as ILogRecordConverter
                };
            }
            catch
            {
                throw;
            }
        }
    }
}
