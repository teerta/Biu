using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using System.Threading;

namespace Biu
{
    public class ClassifiedContainer
    {
        public long ID { get; set; }

        public string Name { get; set; }

        public object Tag { get; set; }


        public ConcurrentDictionary<string, ClassifiedBranch> Branches { get; private set; }


        public ClassifiedContainer()
        {
            this.Branches = new ConcurrentDictionary<string, ClassifiedBranch>();
        }

        public ClassifiedBranch AddBranch(string className, long id, string name)
        {
            if (null == className)
            {
                className = string.Empty;
            }

            if (null == name)
            {
                name = string.Empty;
            }

            var branch = new ClassifiedBranch()
            {
                Class = className,
                ID = id,
                Name = name
            };

            this.Branches.AddOrUpdate(branch.ToString(), branch, (k, v) => { return branch; });

            return branch;
        }

        public ClassifiedBranch QueryGetBranch(string className, long id, string name)
        {
            try
            {
                if (null == className)
                {
                    className = string.Empty;
                }

                if (null == name)
                {
                    name = string.Empty;
                }

                var branch = new ClassifiedBranch()
                {
                    Class = className,
                    ID = id,
                    Name = name
                };

                string strKey = branch.ToString();

                return this.Branches.GetOrAdd(strKey, branch);                
            }
            catch
            {
                throw;
            }
        }
    }
}
