using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;

namespace Biu.WinForm.FunctionView
{
    public class SearchHistory
    {
        public SearchHistory()
        {
            this.LastSearchMode = AppFunctionRowNotifySearchMode.Search;

            this.NavigationItem = new SearchHistoryItem();

            this.Items = new BindingList<SearchHistoryItem>();
        }

        public SearchHistoryItem RestoreTo(int index)
        {
            if (index >= this.Items.Count || index < 0)
            {
                return null;
            }

            int i = this.Items.Count - 1;
            for (; i > index; i--)
            {
                this.Items.RemoveAt(i);
            }

            return this.Items[i];
        }

        public SearchHistoryItem RestoreToLastCommonSearch()
        {
            int i = this.Items.Count - 1;
            for (; i >= 0; i--)
            {
                if (this.Items[i].Type == SearchItemType.CommonSearch)
                {
                    break;
                }

                this.Items.RemoveAt(i);
            }

            return 0 != this.Items.Count ? this.Items[i] : null;
        }

        public void AddItem(SearchHistoryItem item)
        {
            item.ID = this.Items.Count == 0 ? 1 : this.Items[this.Items.Count - 1].ID + 1;
            this.Items.Add(item);
        }

        //public SearchHistoryItem LastItem
        //{
        //    get
        //    {
        //        if (0 == this.Items.Count)
        //        {
        //            var item = new SearchHistoryItem();
        //            this.Items.Add(item);
        //            return item;
        //        }

        //        return this.Items[this.Items.Count - 1];
        //    }
        //}

        public AppFunctionRowNotifySearchMode LastSearchMode { get; set; }

        public SearchHistoryItem CurrentItem { get; set; }

        public SearchHistoryItem NavigationItem { get; set; }


        public BindingList<SearchHistoryItem> Items { get; private set; }
    }
}
