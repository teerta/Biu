using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Runtime.Serialization;
using Biu.Object.Notify;

namespace Biu.WinForm.Input
{
    [Serializable]
    public class ActionItem : NotifiableObject
    {
        public ActionItem()
        {

        }

        #region Property Action
        [DisplayName("动作")]
        [TypeConverter(typeof(ActionTypeConverter))]
        public ActionType Action
        {
            get
            {
                return _Action;
            }
            set
            {
                if (value != _Action)
                {
                    var v = _Action;
                    _Action = value;
                    OnNotifyPropertyChanged("Action", v, _Action);
                }
            }
        }
        private ActionType _Action;
        #endregion // Action

        #region Property SubAction
        [DisplayName("子动作")]
        [TypeConverter(typeof(ActionSubType))]
        public ActionSubType SubAction
        {
            get
            {
                return _SubAction;
            }
            set
            {
                if (value != _SubAction)
                {
                    var v = _SubAction;
                    _SubAction = value;
                    OnNotifyPropertyChanged("SubAction", v, _SubAction);
                }
            }
        }
        private ActionSubType _SubAction;
        #endregion // MouseAction

        #region Property TargetID
        [DisplayName("子动做目标ID")]
        public int TargetID
        {
            get
            {
                return _TargetID;
            }
            set
            {
                if (value != _TargetID)
                {
                    var v = _TargetID;
                    _TargetID = value;
                    OnNotifyPropertyChanged("TargetID", v, _TargetID);
                }
            }
        }
        private int _TargetID;
        #endregion // TargetID

        #region Property Times
        [DisplayName("执行次数")]
        public int Times
        {
            get
            {
                return _Times;
            }
            set
            {
                if (value != _Times)
                {
                    var v = _Times;
                    _Times = value;
                    OnNotifyPropertyChanged("Times", v, _Times);
                }
            }
        }
        private int _Times;
        #endregion // Times

        #region Property WindowClassName
        [DisplayName("窗口类名")]
        public string WindowClassName
        {
            get
            {
                return _WindowClassName;
            }
            set
            {
                if (value != _WindowClassName)
                {
                    var v = _WindowClassName;
                    _WindowClassName = value;
                    OnNotifyPropertyChanged("WindowClassName", v, _WindowClassName);
                }
            }
        }
        private string _WindowClassName;
        #endregion // WindowClassName

        #region Property WindowTitle
        [DisplayName("窗口标题")]
        public string WindowTitle
        {
            get
            {
                return _WindowTitle;
            }
            set
            {
                if (value != _WindowTitle)
                {
                    var v = _WindowTitle;
                    _WindowTitle = value;
                    OnNotifyPropertyChanged("WindowTitle", v, _WindowTitle);
                }
            }
        }
        private string _WindowTitle;
        #endregion // WindowTitle

        #region Property ChildWindowClassName
        [DisplayName("控件类名")]
        public string ChildWindowClassName
        {
            get
            {
                return _ChildWindowClassName;
            }
            set
            {
                if (value != _ChildWindowClassName)
                {
                    var v = _ChildWindowClassName;
                    _ChildWindowClassName = value;
                    OnNotifyPropertyChanged("ChildWindowClassName", v, _ChildWindowClassName);
                }
            }
        }
        private string _ChildWindowClassName;
        #endregion // ChildWindowClassName

        #region Property ChildWindowTitle
        [DisplayName("控件标题")]
        public string ChildWindowTitle
        {
            get
            {
                return _ChildWindowTitle;
            }
            set
            {
                if (value != _ChildWindowTitle)
                {
                    var v = _ChildWindowTitle;
                    _ChildWindowTitle = value;
                    OnNotifyPropertyChanged("ChildWindowTitle", v, _ChildWindowTitle);
                }
            }
        }
        private string _ChildWindowTitle;
        #endregion // ChildWindowTitle

        #region Property X
        [DisplayName("鼠标X位置")]
        public int X
        {
            get
            {
                return _X;
            }
            set
            {
                if (value != _X)
                {
                    var v = _X;
                    _X = value;
                    OnNotifyPropertyChanged("X", v, _X);
                }
            }
        }
        private int _X;
        #endregion // X

        #region Property Y
        [DisplayName("鼠标Y位置")]
        public int Y
        {
            get
            {
                return _Y;
            }
            set
            {
                if (value != _Y)
                {
                    var v = _Y;
                    _Y = value;
                    OnNotifyPropertyChanged("Y", v, _Y);
                }
            }
        }
        private int _Y;
        #endregion // Y

        #region Property Interval
        [DisplayName("执行后延时")]
        public int Interval
        {
            get
            {
                return _Interval;
            }
            set
            {
                if (value != _Interval)
                {
                    var v = _Interval;
                    _Interval = value;
                    OnNotifyPropertyChanged("Interval", v, _Interval);
                }
            }
        }
        private int _Interval;
        #endregion // Interval

        #region Property NamePattern
        [DisplayName("正则表达式名称")]
        public bool NamePattern
        {
            get
            {
                return _NamePattern;
            }
            set
            {
                if (value != _NamePattern)
                {
                    var v = _NamePattern;
                    _NamePattern = value;
                    OnNotifyPropertyChanged("NamePattern", v, _NamePattern);
                }
            }
        }
        private bool _NamePattern;
        #endregion // NamePattern
    }
}
