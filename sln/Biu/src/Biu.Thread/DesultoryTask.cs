using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Concurrent;

namespace Biu.Threading
{
    public enum DesultoryTaskNextFunction
    {
        Next = 1,
        Current,
        Stop,
        Done
    }

    public enum DesultoryTaskRunFunction
    {
        Run = 1,
        Ignore
    }

    public enum DesultoryStatementStackMode
    {
        OneShot = 1,
        Loop,
        Dynamic
    }

    public enum DesultoryBoolean
    {
        True = 1,
        False
    }

    public enum DesultoryTriState
    {
        True = 1,
        False,
        Null
    }


    public class DesultoryCompositeState
    {
        public int ID { get; set; }

        public DesultoryTaskNextFunction NextFunction { get; set; }

        public object ProcessState { get; set; }

        public DesultoryBoolean Boolean { get; set; }
    }


    public class DesultoryStatementCreation
    {
        public Action<object> Statement { get; set; }

        public object State { get; set; }

        public Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> NextCondition { get; set; }
    }


    public class DesultoryStatementStack
    {
        public DesultoryStatementStackMode Mode { get; set; }

        public object RunState { get; set; }


        public DesultoryStatementStack()
        {
            this.Mode = DesultoryStatementStackMode.OneShot;
        }
    }


    public class DesultoryErrorArgument
    {
        public DateTime ErrorDate { get; set; }

        public int RelativeID { get; set; }

        public int RelativeIndex { get; set; }

        public long StepIndicator { get; set; }

        public object StepObjectIndicator { get; set; }

        public string ErrorMessage { get; set; }

        public string AppendixErrorMessage { get; set; }

        public Exception StepException { get; set; }

        public object StepObject { get; set; }

        public object Tag { get; set; }

        public Action<DesultoryStatementBase> ErrorAction { get; set; }


        public DesultoryErrorArgument()
        {
            this.ErrorDate = DateTime.Now;
        }

        public DesultoryErrorArgument(long stepIndicator, string errorMessage = null, string appendixErrorMessage = null)
        {
            this.ErrorDate = DateTime.Now;

            this.StepIndicator = stepIndicator;
            this.ErrorMessage = errorMessage;
            this.AppendixErrorMessage = appendixErrorMessage;
        }

        public bool HandleErrorMessage(string msg)
        {
            if (!string.IsNullOrEmpty(this.ErrorMessage))
            {
                msg = this.ErrorMessage;
                return true;
            }

            if (!string.IsNullOrEmpty(this.AppendixErrorMessage))
            {
                msg = string.Format("{0}{1}{2}", msg, Environment.NewLine, this.AppendixErrorMessage);
                return true;
            }

            return false;
        }
    }


    /// <summary>
    /// Base class for all running items.
    /// </summary>
    public abstract class DesultoryStatementBase
    {
        /// <summary>
        /// Unique ID in a task.
        /// </summary>
        public int ID { get; set; }

        /// <summary>
        /// The task of this item.
        /// </summary>
        public virtual DesultoryTask Desultory { get; set; }

        public virtual int Deep { get; set; }

        /// <summary>
        /// The state is something that caller stores what he wants.
        /// </summary>
        public object State { get; set; }

        /// <summary>
        /// The argument is to transfer parameters from item to item (previous to next).
        /// </summary>
        public object Argument { get; set; }
        
        /// <summary>
        /// U can provide a NextCondition to check if this item can finish.
        /// 
        /// NextCondition is called in CanNext.
        /// </summary>
        /// 
        /// <remarks>
        /// .The DesultoryStatementBase argument is this.
        /// .The object argument is from CanNext by subclasses.
        /// 
        /// ..U should return a DesultoryTaskNextFunction to decide what to do next (CanNext maybe changes this value).
        /// </remarks>
        public Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> NextCondition { get; set; }


        /// <summary>
        /// To decide whether this item is finished and what to do next.
        /// 
        /// This method calls NextCondition if caller providers one.
        /// </summary>
        /// <param name="processState"></param>
        /// <param name="runState"></param>
        /// <returns></returns>
        public virtual DesultoryTaskNextFunction CanNext(object processState, object runState)
        {
            if (null == this.NextCondition)
            {
                return DesultoryTaskNextFunction.Next;
            }

            try
            {
                return this.NextCondition(this, processState);
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// U should overwrite this.
        /// </summary>
        /// <param name="runState"></param>
        /// <returns></returns>
        public abstract int Run(object runState = null);
    }


    /// <summary>
    /// A basic running item for running a Action.
    /// </summary>
    public class DesultoryBlock : DesultoryStatementBase
    {
        public Task Task { get; set; }


        public DesultoryBlock(Action<object> action, CancellationToken cancellationToken)
        {
            this.Task = new Task(action, this, cancellationToken);
        }

        public override int Run(object runState = null)
        {
            try
            {
                this.Task.Start();

                return 1;
            }
            catch
            {
                throw;
            }
        }
    }


    /// <summary>
    /// Run items (item-group) parallelly (each item can loop-run).
    /// </summary>
    /// 
    /// <remarks>
    /// DesultoryParallel.CanNext's runState is a DesultoryCompositeState.
    /// 
    /// DesultoryParallel's sub items should pass its ID as Run's runState.
    /// </remarks>
    public class DesultoryParallel : DesultoryStatementBase
    {
        protected object _syncRoot = new object();
        protected int _remaining;

        public List<DesultoryStatementBase> Tasks { get; protected set; }

        public override DesultoryTask Desultory
        {
            get
            {
                return base.Desultory;
            }
            set
            {
                base.Desultory = value;
                for (int i = 0; i < this.Tasks.Count; i++)
                {
                    this.Tasks[i].Desultory = value;
                }
            }
        }

        public override int Deep
        {
            get
            {
                return base.Deep;
            }
            set
            {
                base.Deep = value;
                for (int i = 0; i < this.Tasks.Count; i++)
                {
                    this.Tasks[i].Deep = this.Deep + 1;
                }
            }
        }


        public DesultoryParallel(IEnumerable<DesultoryStatementBase> items)
        {
            this.Tasks = new List<DesultoryStatementBase>();
            this.Tasks.AddRange(items);
            for (int i = 0; i < this.Tasks.Count; i++)
            {
                this.Tasks[i].ID = i + 1;
            }

            _remaining = this.Tasks.Count;
        }

        public override int Run(object runState = null)
        {
            try
            {
                int nCount = 0;
                if (null == runState)
                {
                    this.Tasks.ForEach(t =>
                    {
                        t.Argument = this.Argument;
                        nCount += t.Run();
                    });
                }
                else
                {
                    int nID = (int)runState;
                    var dti = this.Tasks.Where(t => t.ID == nID).FirstOrDefault();
                    if (null != dti)
                    {
                        dti.Argument = this.Argument;
                        nCount += dti.Run();
                    }
                }
                return nCount;
            }
            catch
            {
                throw;
            }
        }

        public override DesultoryTaskNextFunction CanNext(object processState, object runState)
        {
            try
            {
                int nID = (int)runState;
                var dti = this.Tasks.Where(t => t.ID == nID).FirstOrDefault();
                var compositeState = new DesultoryCompositeState()
                {
                    ID = nID,
                    NextFunction = DesultoryTaskNextFunction.Current,
                    ProcessState = processState
                };

                if (null != dti)
                {
                    var childResult = dti.CanNext(processState, null);
                    if (childResult == DesultoryTaskNextFunction.Done)
                    {
                        lock(_syncRoot)
                        {
                            compositeState.NextFunction = 0 == --_remaining ? DesultoryTaskNextFunction.Next : DesultoryTaskNextFunction.Done;
                        }
                    }
                }

                if (null == this.NextCondition)
                {
                    return compositeState.NextFunction;
                }

                return this.NextCondition(this, compositeState);
            }
            catch
            {
                throw;
            }            
        }
    }  


    public class DesultoryIf : DesultoryStatementBase
    {
        internal int ID_True = 1;
        internal int ID_False = 2;


        public DesultoryStatementBase TrueStatement { get; set; }

        public DesultoryStatementBase FalseStatement { get; set; }


        public override DesultoryTask Desultory
        {
            get
            {
                return base.Desultory;
            }
            set
            {
                base.Desultory = value;
                this.TrueStatement.Desultory = this.FalseStatement.Desultory = value;
            }
        }

        public override int Deep
        {
            get
            {
                return base.Deep;
            }
            set
            {
                base.Deep = value;
                this.TrueStatement.Deep = this.FalseStatement.Deep = this.Deep + 1;
            }
        }

        public DesultoryIf(Func<DesultoryIf, object, DesultoryBoolean> ifCondition, 
            DesultoryStatementBase trueStatement, DesultoryStatementBase falseStatement = null)
        {
            if (null == ifCondition)
            {
                throw new ArgumentNullException("ifCondition is null");
            }

            if (null == trueStatement)
            {
                throw new ArgumentNullException("trueStatement is null");
            }

            this.IfCondition = IfCondition;

            trueStatement.ID = ID_True;
            this.TrueStatement = trueStatement;

            if (null != falseStatement)
            {
                falseStatement.ID = ID_False;
                this.FalseStatement = falseStatement;
            }            
        }

        public override int Run(object runState = null)
        {
            try
            {
                int nCount = 0;
                if (this.IfCondition(this, runState) == DesultoryBoolean.True)
                {
                    this.TrueStatement.Argument = this.Argument;
                    nCount += this.TrueStatement.Run(runState);
                }
                else
                {
                    if (null != this.FalseStatement)
                    {
                        this.FalseStatement.Argument = this.Argument;
                        nCount += this.FalseStatement.Run(runState);
                    }
                }

                return nCount;
            }
            catch
            {
                throw;
            }
        }

        public override DesultoryTaskNextFunction CanNext(object processState, object runState)
        {
            try
            {
                var compositeState = new DesultoryCompositeState()
                {
                    ID = ID_True,
                    NextFunction = DesultoryTaskNextFunction.Next,
                    ProcessState = processState,
                    Boolean = this.IfCondition(this, runState)
                };
                
                if (DesultoryBoolean.True == compositeState.Boolean)
                {
                    compositeState.NextFunction = this.TrueStatement.CanNext(processState, null);
                }
                else
                {
                    if (null != this.FalseStatement)
                    {
                        compositeState.NextFunction = this.FalseStatement.CanNext(processState, null);
                    }
                }

                if (compositeState.NextFunction == DesultoryTaskNextFunction.Done)
                {
                    compositeState.NextFunction = DesultoryTaskNextFunction.Next;
                }

                if (null == this.NextCondition)
                {
                    return compositeState.NextFunction;
                }

                return this.NextCondition(this, compositeState);
            }
            catch
            {
                throw;
            }            
        }

        public Func<DesultoryIf, object, DesultoryBoolean> IfCondition { get; set; }
    }


    /// <summary>
    /// A task is something like a function who can run any number of items.
    /// </summary>
    /// 
    /// <remarks>
    /// A task has a finished item and a error item.
    /// 
    /// Finished-item is executed when every item of task has been finished and no-error was happened.
    /// If task's constructor has no finished-item, this class create a default one to clear resources.
    /// If task's constructor has a finished-item, the class create a continued task to clear resources.
    /// 
    /// Error-item can be triggered by each item of task when it decides to do.
    /// </remarks>
    public class DesultoryTask : IDisposable
    {
        protected object _syncRoot = new object();
        protected DesultoryBlock _finishedItem;
        protected DesultoryBlock _errorItem;
        protected int _currentIndex = -1;
        protected bool _done;
        protected int _pendingItemCount = 0;

        protected List<DesultoryStatementBase> _items = new List<DesultoryStatementBase>();
        protected List<DesultoryStatementStack> _stacks = new List<DesultoryStatementStack>();
        


        public int ID { get; set; }

        public object Argument { get; set; }

        public bool IsFailed { get; set; }

        public CancellationTokenSource CTSource { get; private set; }


        public DesultoryTask()
        {
            this.CTSource = new CancellationTokenSource();
        }

        ~DesultoryTask()
        {
            Dispose(false);
        }

        public void Create(IEnumerable<DesultoryStatementBase> items, DesultoryBlock finishedItem = null, DesultoryBlock errorItem = null)
        {
            try
            {
                foreach (var item in items)
                {
                    item.Desultory = this;
                    item.Deep = 1;
                    _items.Add(item);
                }

                Init(finishedItem, errorItem);
            }
            catch
            {
                throw;
            }
        }

        public bool Run(object processState = null, object argument = null, bool failed = false, object runState = null)
        {
            try
            {
                if (0 == _items.Count)
                {
                    throw new ArgumentException("No tasks to run");
                }

                bool bRet = true;
                lock(_syncRoot)
                {
                    if (this.IsFailed)
                    {
                        _pendingItemCount--;
                        return false;
                    }
                
                    if (failed)
                    {
                        this.IsFailed = true;
                        if (null != _errorItem)
                        {
                            _errorItem.Argument = argument;
                            _errorItem.Run();
                        }
                    }
                    else
                    {                        
                        DesultoryStatementBase item = null;
                        if (-1 == _currentIndex)
                        {
                            item = _items[++_currentIndex];
                        }
                        else
                        {
                            _pendingItemCount--;
                            item = _items[_currentIndex];
                            switch (item.CanNext(processState, runState))
                            {
                                case DesultoryTaskNextFunction.Next:
                                    {
                                        if (_currentIndex == _items.Count - 1)
                                        {
                                            item = null;
                                            if (!_done)
                                            {
                                                if (0 == _pendingItemCount)
                                                {
                                                    _done = true;
                                                    if (null != _finishedItem)
                                                    {
                                                        _finishedItem.Run();
                                                    }
                                                }
                                            }

                                            bRet = false;
                                        }
                                        else
                                        {
                                            item = _items[++_currentIndex];
                                        }
                                        _stacks.Clear();
                                    }
                                    break;

                                case DesultoryTaskNextFunction.Current:
                                    item = _items[_currentIndex];
                                    break;

                                case DesultoryTaskNextFunction.Stop:
                                case DesultoryTaskNextFunction.Done:
                                    item = null;
                                    _stacks.Clear();
                                    break;
                            }
                        }                    

                        if (null == item)
                        {
                            bRet = false;
                        }
                        else
                        {
                            item.Argument = argument;
                            int nCount = item.Run(runState);
                            if (0 == nCount)
                            {
                                bRet = false;
                            }
                            else
                            {
                                _pendingItemCount += nCount;
                            }
                        }                        
                    }
                }

                return bRet;
            }
            catch
            {
                throw;
            }
        }

        public void Finish()
        {
            try
            {
                if (null != _finishedItem)
                {
                    _finishedItem.Task.Start();
                }
            }
            catch
            {
                throw;
            }
        }


        #region IDisposable
        private bool disposed = false;

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {

            }

            this.CTSource.Dispose();

            disposed = true;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }
        #endregion // IDisposable        


        #region Creation Utils

        /// <summary>
        /// Create a new task and reverse a task-id for it.
        /// </summary>
        /// <param name="argument"></param>
        /// <returns></returns>
        public static DesultoryTask NewTask(object argument = null)
        {
            var dtask = new DesultoryTask()
            {
                ID = DesultoryTaskManager.Instance.Reserve(),
                Argument = argument
            };

            return dtask;
        }

        protected void Init(DesultoryBlock finishedItem, DesultoryBlock errorItem)
        {
            if (null != finishedItem)
            {
                _finishedItem = finishedItem;
                _finishedItem.Desultory = this;
                _finishedItem.Deep = 1;
            }

            if (null != errorItem)
            {
                _errorItem = errorItem;
                _errorItem.Desultory = this;
                _errorItem.Deep = 1;
            }

            if (null == _finishedItem)
            {
                _finishedItem = new DesultoryBlock((arg) =>
                {
                    var argDTI = arg as DesultoryBlock;

                    DesultoryTaskManager.Instance.Remove(argDTI.Desultory.ID);
                }, this.CTSource.Token);
            }
            else
            {
                _finishedItem.Task.ContinueWith((argTask, arg) =>
                {
                    var dtask = arg as DesultoryTask;

                    DesultoryTaskManager.Instance.Remove(dtask.ID);
                }, this, this.CTSource.Token);
            }
        }

        /// <summary>
        /// Dispatch this task (run it).
        /// 
        /// U should call this method after a series of creations.
        /// This methid also initializes finished-item and error-item.
        /// </summary>
        /// <param name="finishedAction"></param>
        /// <param name="errorAction"></param>
        /// <param name="finishedState"></param>
        /// <param name="errorState"></param>
        public void Dispatch(Action<object> finishedAction = null, Action<object> errorAction = null, object finishedState = null, object errorState = null)
        {
            try
            {
                DesultoryBlock finishedItem = null;
                DesultoryBlock errorItem = null;
                if (null != finishedAction)
                {
                    finishedItem = new DesultoryBlock(finishedAction, this.CTSource.Token)
                    {
                        Desultory = this,
                        State = finishedState
                    };
                }

                if (null != errorAction)
                {
                    errorItem = new DesultoryBlock(errorAction, this.CTSource.Token)
                    {
                        Desultory = this,
                        State = errorState
                    };
                }

                Init(finishedItem, errorItem);

                DesultoryTaskManager.Instance.Add(this.ID, this);
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Create a DesultoryStatementBase-inhereted object and add it to this task.
        /// </summary>
        /// <param name="create"></param>
        /// <returns></returns>
        public DesultoryTask WithStatement(Func<DesultoryTask, DesultoryStatementBase> create)
        {
            var item = create(this);
            item.Desultory = this;
            item.Deep = 1;
            _items.Add(item);

            return this;
        }

        /// <summary>
        /// Create a DesultoryBlock and add it to this task.
        /// </summary>
        /// <param name="action"></param>
        /// <param name="state"></param>
        /// <param name="nextCondition"></param>
        /// <returns></returns>
        public DesultoryTask WithBlock(Action<object> action, object state = null, 
            Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> nextCondition = null)
        {
            var block = new DesultoryBlock(action, this.CTSource.Token)
            {
                Desultory = this,
                Deep = 1,
                State = state,
                NextCondition = nextCondition
            };

            _items.Add(block);

            return this;
        }

        /// <summary>
        /// Create a DesultoryParallel with shared behaviors and add it to this task.
        /// </summary>
        /// <param name="actions"></param>
        /// <param name="sharedState"></param>
        /// <param name="sharedNextCondition"></param>
        /// <param name="parallelState"></param>
        /// <param name="parallelNextCondition"></param>
        /// <returns></returns>
        public DesultoryTask WithParallelBlock(IEnumerable<Action<object>> actions, 
            object sharedState = null, Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> sharedNextCondition = null,
            object parallelState = null, Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> parallelNextCondition = null)
        {
            var items = new List<DesultoryBlock>();
            foreach (var action in actions)
            {
                items.Add(new DesultoryBlock(action, this.CTSource.Token)
                {
                    State = sharedState,
                    NextCondition = sharedNextCondition
                });
            }

            var parallel = new DesultoryParallel(items)
            {
                Desultory = this,
                Deep = 1,
                State = parallelState,
                NextCondition = parallelNextCondition
            };
            _items.Add(parallel);

            return this;
        }

        /// <summary>
        /// Create a DesultoryParallel with multiple same DesultoryBlocks and add it to this task.
        /// </summary>
        /// <param name="count"></param>
        /// <param name="action"></param>
        /// <param name="state"></param>
        /// <param name="nextCondition"></param>
        /// <param name="parallelState"></param>
        /// <param name="parallelNextCondition"></param>
        /// <returns></returns>
        public DesultoryTask WithParallelBlock(int count, Action<object> action, 
            object state = null, Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> nextCondition = null, 
            object parallelState = null, Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> parallelNextCondition = null)
        {
            var items = new List<DesultoryBlock>();
            for (int i = 0; i < count; i++)
            {
                items.Add(new DesultoryBlock(action, this.CTSource.Token)
                {
                    State = state,
                    NextCondition = nextCondition
                });
            }

            var parallel = new DesultoryParallel(items)
            {
                Desultory = this,
                Deep = 1,
                State = parallelState,
                NextCondition = parallelNextCondition
            };
            _items.Add(parallel);

            return this;
        }

        /// <summary>
        /// Create a DesultoryParallel with any uncorrelated DesultoryBlocks and add it to this task.
        /// </summary>
        /// <param name="creations"></param>
        /// <param name="parallelState"></param>
        /// <param name="parallelNextCondition"></param>
        /// <returns></returns>
        public DesultoryTask WithParallelBlock(IEnumerable<DesultoryStatementCreation> creations,
            object parallelState = null, Func<DesultoryStatementBase, object, DesultoryTaskNextFunction> parallelNextCondition = null)
        {
            var items = new List<DesultoryBlock>();
            foreach (var c in creations)
            {
                items.Add(new DesultoryBlock(c.Statement, this.CTSource.Token)
                {
                    State = c.State,
                    NextCondition = c.NextCondition
                });
            }

            var parallel = new DesultoryParallel(items)
            {
                Desultory = this,
                Deep = 1,
                State = parallelState,
                NextCondition = parallelNextCondition
            };
            _items.Add(parallel);

            return this;
        }
        #endregion
    }


    /// <summary>
    /// A singleton class that is to manage all DesultoryTasks.
    /// 
    /// Each task has a unique task-id.
    /// </summary>
    public class DesultoryTaskManager
    {
        #region Singleton
        private static readonly Lazy<DesultoryTaskManager> _lazyInstance = 
            new Lazy<DesultoryTaskManager>(() => { return new DesultoryTaskManager(); });

        public static DesultoryTaskManager Instance { get { return _lazyInstance.Value; } }
        #endregion //Singleton


        public ConcurrentDictionary<int, DesultoryTask> Tasks { get; private set; }


        private DesultoryTaskManager()
        {
            this.Tasks = new ConcurrentDictionary<int, DesultoryTask>();
        }

        /// <summary>
        /// Reserve a task-id.
        /// </summary>
        /// <returns></returns>
        public int Reserve()
        {
            try
            {
                int nID = NextID();
                this.Tasks.AddOrUpdate(nID, new DesultoryTask() { ID = nID }, (k, v) => { return v; });

                return nID;
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Add a task with a reserved task-id.
        /// </summary>
        /// <param name="reservedID"></param>
        /// <param name="task"></param>
        /// <param name="run"></param>
        public void Add(int reservedID, DesultoryTask task, bool run = true)
        {
            try
            {
                task.ID = reservedID;
                this.Tasks.AddOrUpdate(reservedID, task, (k, v) => { return task; });
                if (run)
                {
                    task.Run();
                }
            }
            catch
            {
                throw;
            }
        }

        /// <summary>
        /// Add a task.
        /// </summary>
        /// <param name="task"></param>
        /// <param name="run"></param>
        /// <returns></returns>
        public int Add(DesultoryTask task, bool run = true)
        {
            try
            {
                int nID = NextID();
                task.ID = nID;
                this.Tasks.AddOrUpdate(nID, task, (k, v) => { return task; });
                if (run)
                {
                    task.Run();
                }

                return nID;
            }
            catch
            {
                throw;
            }
        }

        public DesultoryTask Get(int id)
        {
            try
            {
                if (!this.Tasks.ContainsKey(id))
                {
                    return null;
                }

                DesultoryTask task;
                while (!this.Tasks.TryGetValue(id,out task))
                {
                    Thread.Sleep(50);
                }

                return task;
            }
            catch
            {
                throw;
            }
        }

        public void Remove(int id)
        {
            try
            {
                if (!this.Tasks.ContainsKey(id))
                {
                    return;
                }

                DesultoryTask task;
                while (!this.Tasks.TryRemove(id, out task))
                {
                    Thread.Sleep(50);
                }

                return;
            }
            catch
            {
                throw;
            }
        }

        public void Remove(DesultoryTask task)
        {
            try
            {
                Remove(task.ID);
            }
            catch
            {
                throw;
            }
        }

        private int NextID()
        {
            var rand = new Random();
            int nID = 0;
            do
            {
                nID = rand.Next(100, 999999);
            } while (this.Tasks.ContainsKey(nID));

            return nID;
        }        
    }
}
