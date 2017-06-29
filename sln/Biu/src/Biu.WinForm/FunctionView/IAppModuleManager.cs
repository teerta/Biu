using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Biu.Transmission;

namespace Biu.WinForm.FunctionView
{
    public interface IAppModuleManager
    {
        ModuleTrait Traits { get; }


        bool Init();

        void Uninit();

        bool SessionInit();

        void SessionUninit();

        void Run();

        IOOperation BiuHopIo(string name, ulong action, Peer peer, ref bool handled);

        void BiuEnterHop();
        
        void BiuLeaveHop();
    }
}
