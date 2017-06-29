using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Biu.WinForm.Input
{
    [TypeConverter(typeof(ActionTypeConverter))]
    public enum ActionType : int
    {
        InputMouseMove = 1,
        InputMouseMoveAbsolute,
        InputMouseLeftDown,
        InputMouseLeftUp,
        InputMouseRightDown,
        InputMouseRightUp,
        InputMouseMiddleDown,
        InputMouseMiddleUp,

        MsgMouseLeft,
        MsgMouseRight,

        Message,
        MsgCommand
    }

    [TypeConverter(typeof(ActionSubTypeConverter))]
    public enum ActionSubType
    {       
        CommandMenu = 1,
        CommandAccelerator,
        CommandControl
    }
}
