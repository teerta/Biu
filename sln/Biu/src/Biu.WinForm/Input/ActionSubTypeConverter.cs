using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Biu.WinForm.Input
{
    public class ActionSubTypeConverter : EnumConverter
    {
        public const string Val_CommandMenu = "菜单命令";
        public const string Val_CommandAccelerator = "快捷键命令";
        public const string Val_CommandControl = "控件命令";

        public ActionSubTypeConverter()
            : base(typeof(ActionSubType))
        {

        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(ActionSubType))
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
                switch ((ActionSubType)value)
                {
                    case ActionSubType.CommandMenu:
                        strRet = Val_CommandMenu;
                        break;

                    case ActionSubType.CommandAccelerator:
                        strRet = Val_CommandAccelerator;
                        break;

                    case ActionSubType.CommandControl:
                        strRet = Val_CommandControl;
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
                ActionSubType ret = ActionSubType.CommandMenu;
                switch ((string)value)
                {
                    case Val_CommandAccelerator:
                        ret = ActionSubType.CommandAccelerator;
                        break;

                    case Val_CommandControl:
                        ret = ActionSubType.CommandControl;
                        break;
                }

                return ret;
            }

            return base.ConvertFrom(context, culture, value);
        }
    }
}
