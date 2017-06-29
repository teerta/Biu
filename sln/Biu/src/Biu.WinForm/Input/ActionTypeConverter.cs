using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Biu.WinForm.Input
{
    public class ActionTypeConverter : EnumConverter
    {
        public const string Val_InputMouseMove = "鼠标移动（相对位置）";
        public const string Val_InputMouseMoveAbsolute = "鼠标移动（绝对位置）";
        public const string Val_InputMouseLeftDown = "鼠标左键按下";
        public const string Val_InputMouseLeftUp = "鼠标左键释放";
        public const string Val_InputMouseRightDown = "鼠标右键按下";
        public const string Val_InputMouseRightUp = "鼠标右键释放";
        public const string Val_InputMouseMiddleDown = "鼠标滚轮按下";
        public const string Val_InputMouseMiddleUp = "鼠标滚轮释放";
        public const string Val_MsgMouseLeft = "消息-鼠标左键";
        public const string Val_MsgMouseRight = "消息-鼠标右键";
        public const string Val_Message = "消息";
        public const string Val_MsgCommand = "消息-命令";

        public ActionTypeConverter()
            : base(typeof(ActionType))
        {

        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(ActionType))
            {
                return true;
            }

            return base.CanConvertTo(context, destinationType);
        }

        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (destinationType == typeof(string))
            {
                string strRet = string.Empty;
                switch ((ActionType)value)
                {
                    case ActionType.InputMouseMove:
                        strRet = Val_InputMouseMove;
                        break;

                    case ActionType.InputMouseMoveAbsolute:
                        strRet = Val_InputMouseMoveAbsolute;
                        break;

                    case ActionType.InputMouseLeftDown:
                        strRet = Val_InputMouseLeftDown;
                        break;

                    case ActionType.InputMouseLeftUp:
                        strRet = Val_InputMouseLeftUp;
                        break;

                    case ActionType.InputMouseRightDown:
                        strRet = Val_InputMouseRightDown;
                        break;

                    case ActionType.InputMouseRightUp:
                        strRet = Val_InputMouseRightUp;
                        break;

                    case ActionType.InputMouseMiddleDown:
                        strRet = Val_InputMouseMiddleDown;
                        break;

                    case ActionType.InputMouseMiddleUp:
                        strRet = Val_InputMouseMiddleUp;
                        break;

                    case ActionType.MsgMouseLeft:
                        strRet = Val_MsgMouseLeft;
                        break;

                    case ActionType.MsgMouseRight:
                        strRet = Val_MsgMouseRight;
                        break;

                    case ActionType.Message:
                        strRet = Val_Message;
                        break;

                    case ActionType.MsgCommand:
                        strRet = Val_MsgCommand;
                        break;
                }

                return strRet;
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(string))
            {
                return true;
            }

            return base.CanConvertFrom(context, sourceType);
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            if (value is string)
            {
                ActionType ret = ActionType.InputMouseMove;
                switch ((string)value)
                {
                    case Val_InputMouseMoveAbsolute:
                        ret = ActionType.InputMouseMoveAbsolute;
                        break;

                    case Val_InputMouseLeftDown:
                        ret = ActionType.InputMouseLeftDown;
                        break;

                    case Val_InputMouseLeftUp:
                        ret = ActionType.InputMouseLeftUp;
                        break;

                    case Val_InputMouseRightDown:
                        ret = ActionType.InputMouseRightDown;
                        break;

                    case Val_InputMouseRightUp:
                        ret = ActionType.InputMouseRightUp;
                        break;

                    case Val_InputMouseMiddleDown:
                        ret = ActionType.InputMouseMiddleUp;
                        break;

                    case Val_InputMouseMiddleUp:
                        ret = ActionType.InputMouseMiddleUp;
                        break;

                    case Val_MsgMouseLeft:
                        ret = ActionType.MsgMouseLeft;
                        break;

                    case Val_MsgMouseRight:
                        ret = ActionType.MsgMouseRight;
                        break;

                    case Val_Message:
                        ret = ActionType.Message;
                        break;

                    case Val_MsgCommand:
                        ret = ActionType.MsgCommand;
                        break;
                }

                return ret;
            }

            return base.ConvertFrom(context, culture, value);
        }
    }
}
