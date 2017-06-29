using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace Biu.Serialization
{
    public class BinderReplacedItem
    {
        public string AssemblyName { get; set; }

        public string TypeName { get; set; }

        public Type ToType { get; set; }

        public BinderNameMatchFunction AssemblyMatchFunction { get; set; }

        public BinderNameMatchFunction MatchFunction { get; set; }

        public System.Text.RegularExpressions.RegexOptions RegularExpressionOptions { get; set; }


        public BinderReplacedItem()
        {
            this.AssemblyMatchFunction = BinderNameMatchFunction.Equal;
            this.MatchFunction = BinderNameMatchFunction.Equal;
            this.RegularExpressionOptions = RegexOptions.None;
        }

        public bool IsMatch(string assemblyName, string typeName)
        {
            try
            {
                return MatchAssembly(assemblyName) && MatchType(typeName);
            }
            catch
            {
                throw;
            }
        }

        public bool MatchAssembly(string assemblyName)
        {
            try
            {
                if (string.IsNullOrEmpty(this.AssemblyName))
                {
                    return false;
                }

                return Match(this.AssemblyName, assemblyName, this.AssemblyMatchFunction);
            }
            catch
            {
                throw;
            }
        }

        public bool MatchType(string typeName)
        {
            try
            {
                if (string.IsNullOrEmpty(this.TypeName))
                {
                    return false;
                }

                return Match(this.TypeName, typeName, this.MatchFunction);
            }
            catch
            {
                throw;
            }
        }

        private bool Match(string arg, string name, BinderNameMatchFunction matchFunction)
        {
            try
            {
                bool bRet = false;
                switch (matchFunction)
                {
                    case BinderNameMatchFunction.Equal:
                        bRet = arg == name;
                        break;

                    case BinderNameMatchFunction.StartWith:
                        bRet = name.StartsWith(arg);
                        break;

                    case BinderNameMatchFunction.EndWith:
                        bRet = name.EndsWith(arg);
                        break;

                    case BinderNameMatchFunction.IsSuperset:
                        bRet = name.Contains(arg);
                        break;

                    case BinderNameMatchFunction.IsSubset:
                        bRet = arg.Contains(name);
                        break;

                    case BinderNameMatchFunction.RegularExpression:
                        bRet = (new Regex(arg, this.RegularExpressionOptions)).IsMatch(name);
                        break;
                }

                return bRet;
            }
            catch
            {
                throw;
            }
        }
    }
}
