using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Diagnostics
{
    public interface ILogRecorder
    {
        bool IsAvailable { get; set; }

        ILogRecordConverter RecordConverter { get; set; }

        bool Initialize();

        void Uninitialize();

        bool Open(object arg);

        void Close();

        void Record(LogRecord logRecord);
    }
}
