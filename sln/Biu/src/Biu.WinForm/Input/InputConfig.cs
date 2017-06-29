using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Runtime.Serialization.Formatters.Binary;
using Biu.Object.Notify;
using Biu.Serialization;

namespace Biu.WinForm.Input
{
    [Serializable]
    public class InputConfig : NotifiableObject
    {
        #region Property ID
        public Guid ID
        {
            get
            {
                return _ID;
            }
            set
            {
                if (value != _ID)
                {
                    var v = _ID;
                    _ID = value;
                    OnNotifyPropertyChanged("ID", v, _ID);
                }
            }
        }
        private Guid _ID;
        #endregion // ID

        #region Property Name
        public string Name
        {
            get
            {
                return _Name;
            }
            set
            {
                if (value != _Name)
                {
                    var v = _Name;
                    _Name = value;
                    OnNotifyPropertyChanged("Name", v, _Name);
                }
            }
        }
        private string _Name;
        #endregion // Name

        #region Property Ver
        public string Ver
        {
            get
            {
                return _Ver;
            }
            set
            {
                if (value != _Ver)
                {
                    var v = _Ver;
                    _Ver = value;
                    OnNotifyPropertyChanged("Ver", v, _Ver);
                }
            }
        }
        private string _Ver;
        #endregion // Ver

        #region Property Family
        public string Family
        {
            get
            {
                return _Family;
            }
            set
            {
                if (value != _Family)
                {
                    var v = _Family;
                    _Family = value;
                    OnNotifyPropertyChanged("Family", v, _Family);
                }
            }
        }
        private string _Family;
        #endregion // Family

        #region Property AppName
        public string AppName
        {
            get
            {
                return _AppName;
            }
            set
            {
                if (value != _AppName)
                {
                    var v = _AppName;
                    _AppName = value;
                    OnNotifyPropertyChanged("AppName", v, _AppName);
                }
            }
        }
        private string _AppName;
        #endregion // AppName

        #region Property AppVer
        public string AppVer
        {
            get
            {
                return _AppVer;
            }
            set
            {
                if (value != _AppVer)
                {
                    var v = _AppVer;
                    _AppVer = value;
                    OnNotifyPropertyChanged("AppVer", v, _AppVer);
                }
            }
        }
        private string _AppVer;
        #endregion // AppVer

        #region Property LastModified
        public DateTimeOffset LastModified
        {
            get
            {
                return _LastModified;
            }
            set
            {
                if (value != _LastModified)
                {
                    var v = _LastModified;
                    _LastModified = value;
                    OnNotifyPropertyChanged("LastModified", v, _LastModified);
                }
            }
        }
        private DateTimeOffset _LastModified;
        #endregion // LastModified

        public BindingList<ActionItem> Actions { get; private set; }


        public InputConfig()
        {
            this.Actions = new BindingList<ActionItem>();

            this.ID = Guid.NewGuid();
        }

        public void Save(string path)
        {
            try
            {
                SerializationUtil.SerializeToFile<BinaryFormatter>(path, this);
            }
            catch
            {
                throw;
            }
        }

        public static InputConfig Load(string path)
        {
            try
            {
                return SerializationUtil.DeserializeFromFile<BinaryFormatter, InputConfig>(path);
            }
            catch
            {
                throw;
            }
        }
    }
}
