using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.Serialization
{
    public enum PersistentItemSignificance
    {
        Normal = 1,
        Important
    }

    public enum PersistentItemStoredType
    {
        File = 1,
        Stream = 2,
        Binary
    }

    public enum PartitionPlanMethod
    {
        Average = 1,
        HeadAndRemaining,
        TailAndRemaining,
        HeadTailAndRemaining
    }

    public enum PersistentItemType
    {
        Content = 1,
        Head,
        Tail
    }

    public enum BinderNameMatchFunction
    {
        /// <summary>
        /// Name equals to arg.
        /// </summary>
        Equal = 1,

        /// <summary>
        /// Name starts with arg.
        /// </summary>
        StartWith,

        /// <summary>
        /// Name ends with arg;
        /// </summary>
        EndWith,

        /// <summary>
        /// Name is superset of arg.
        /// </summary>
        IsSuperset,

        /// <summary>
        /// Name is subset of arg.
        /// </summary>
        IsSubset,

        /// <summary>
        /// Arg is reg-pattern that name matches to.
        /// </summary>
        RegularExpression
    }
}
