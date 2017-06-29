using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Diagnostics
{
    public interface ILogRecordConverter
    {

    }

    public interface ILogRecordConverter<T> : ILogRecordConverter
    {
        T ConvertTo(LogRecord logRecord);
    }
}
