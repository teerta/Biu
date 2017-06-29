using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Biu.WinForm.FunctionView
{
    public enum SearchItemType
    {
        CommonSearch = 1,
        AutoNewRow
    }


    public class SearchHistoryItem
    {
        public SearchHistoryItem()
        {
            this.Time = DateTime.Now;
            this.Type = SearchItemType.CommonSearch;

            this.SearchParameters = new Dictionary<string, object>();
        }

        public int ID { get; set; }

        public DateTime Time { get; set; }

        public string Description { get; set; }

        public long TotalRowCount { get; set; }

        public long CurrentRowCount { get; set; }

        public long ActiveRowIndex { get; set; }

        public long ActiveRowID { get; set; }

        public SearchItemType Type { get; set; }

        public object Rows { get; set; }

        public object Tag { get; set; }

        public Dictionary<string, object> SearchParameters { get; private set; }


        public bool IsMoreRows
        {
            get
            {
                return this.TotalRowCount != this.CurrentRowCount;
            }
        }


        public void DecreaseCount(long count = 1)
        {
            this.TotalRowCount -= count;
            this.CurrentRowCount -= count;
        }

        public void IncreaseCount(long count = 1)
        {
            this.TotalRowCount += count;
            this.CurrentRowCount += count;
        }
    }
}
