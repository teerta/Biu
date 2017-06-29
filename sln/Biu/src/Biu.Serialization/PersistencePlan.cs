using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Biu.Native.Security;

namespace Biu.Serialization
{
	/// <summary>
	/// Plan for persistence.
	/// </summary>
	public class PersistencePlan
	{
		public PartitionPlanMethod Method { get; set; }

		/// <summary>
		/// Size of a part (as is a PersistenceItem)
		/// </summary>
		public long Size { get; set; }

		/// <summary>
		/// Size of header of data.
		/// </summary>
		public long HeadSize { get; set; }

		/// <summary>
		/// Size of tail of data.
		/// </summary>
		public long TailSize { get; set; }


		/// <summary>
		/// All formatters that a persistence needs.
		/// </summary>
		public List<IPersistentItemFormatter> Formatters { get; private set; }

		/// <summary>
		/// All keys that a persistence needs.
		/// </summary>
		public List<CryptKey> Keys { get; private set; }


		public PersistencePlan()
		{
			this.Formatters = new List<IPersistentItemFormatter>();
			this.Keys = new List<CryptKey>();
		}

		#region Public Virtual
		/// <summary>
		/// Determinate how to save a ItemPartition.
		/// Subclass should override this method to meet its needs.
		/// </summary>
		/// <param name="partition"></param>
		/// <returns></returns>
		/// <see cref="Biu.Serialization.PersistenceDecision"/>
		/// <see cref="Biu.Serialization.ItemPartition"/>
		public virtual PersistenceDecision MakeSavingDecision(ItemPartition partition)
		{            
			return new PersistenceDecision()
			{
				Key = this.Keys.FirstOrDefault(),
				Formatter = this.Formatters.FirstOrDefault(),
				StoredType = PersistentItemStoredType.Stream,
				Significance = PersistentItemSignificance.Normal
			};
		}

		/// <summary>
		/// Determinate how to load a PersistenceItem.
		/// Subclass should override this method to meet its needs.
		/// </summary>
		/// <param name="item"></param>
		/// <returns></returns>
		/// /// <see cref="Biu.Serialization.PersistenceDecision"/>
		/// <see cref="Biu.Serialization.PersistentItem"/>
		public virtual PersistenceDecision MakeLoadingDecision(PersistentItem item)
		{
			return new PersistenceDecision()
			{
				Key = this.Keys.FirstOrDefault(),
				Formatter = this.Formatters.FirstOrDefault()
			};
		}
		#endregion

		/// <summary>
		/// Partition a stream.
		/// This method is for PersistenceBase and its subclasses.
		/// </summary>
		/// <param name="stream"></param>
		/// <returns></returns>
		public IEnumerable<ItemPartition> Partition(Stream stream)
		{
			long oldPos = stream.Position;
			try
			{
				List<ItemPartition> partitions = new List<ItemPartition>();
				switch (this.Method)
				{
					case PartitionPlanMethod.Average:
						AveragePartition(stream, partitions);
						break;

					case PartitionPlanMethod.HeadAndRemaining:
						HeadPartition(stream, partitions);
						break;

					case PartitionPlanMethod.TailAndRemaining:
						TailPartition(stream, partitions);
						break;

					case PartitionPlanMethod.HeadTailAndRemaining:
						HeadTailPartition(stream, partitions);
						break;
				}

				return partitions;
			}
			catch
			{
				throw;
			}
			finally
			{
				stream.Seek(oldPos, SeekOrigin.Begin);
			}
		}

		protected void AveragePartition(Stream stream, List<ItemPartition> list)
		{
			try
			{
				long nCount = (stream.Length - stream.Position) / this.Size;
				if (0 != (stream.Length - stream.Position) % this.Size)
				{
					nCount++;
				}

				for (long i = 0; i < nCount; i++)
				{
					var item = new ItemPartition()
					{
						ItemType = PersistentItemType.Content,
						Position = stream.Position + i * this.Size
					};
					item.Size = Math.Min(this.Size, stream.Length - item.Position);

					list.Add(item);
				}
			}
			catch
			{
				throw;
			}
		}

		protected void HeadPartition(Stream stream, List<ItemPartition> list)
		{
			try
			{
				var head = new ItemPartition()
				{
					ItemType = PersistentItemType.Head,
					Position = stream.Position
				};
				head.Size = Math.Min(this.HeadSize, stream.Length - head.Position);
				list.Add(head);

				var item = new ItemPartition()
				{
					ItemType = PersistentItemType.Content,
					Position = head.EndPosition
				};
				item.Size = stream.Length - item.Position;
				list.Add(item);
			}
			catch
			{
				throw;
			}
		}

		protected void TailPartition(Stream stream, List<ItemPartition> list)
		{
			try
			{
				var tail = new ItemPartition()
				{
					ItemType = PersistentItemType.Tail,
					Position = stream.Length - this.TailSize,
					Size = this.TailSize
				};
				list.Add(tail);

				var item = new ItemPartition()
				{
					ItemType = PersistentItemType.Content,
					Position = stream.Position
				};
				item.Size = stream.Length - stream.Position - tail.Size;
				list.Add(item);
			}
			catch
			{
				throw;
			}
		}

		protected void HeadTailPartition(Stream stream, List<ItemPartition> list)
		{
			try
			{
				var head = new ItemPartition()
				{
					ItemType = PersistentItemType.Head,
					Position = stream.Position
				};
				head.Size = Math.Min(this.HeadSize, stream.Length - head.Position);
				list.Add(head);

				var tail = new ItemPartition()
				{
					ItemType = PersistentItemType.Tail,
					Position = stream.Length - this.TailSize,
					Size = this.TailSize
				};

				long nRemainingSize = stream.Length - stream.Position - head.Size - tail.Size;
				long nCount = nRemainingSize / this.Size;
				if (0 != nRemainingSize % this.Size)
				{
					nCount++;
				}

				for (long i = 0; i < nCount; i++)
				{
					var item = new ItemPartition()
					{
						ItemType = PersistentItemType.Content,
						Position = stream.Position + i * this.Size
					};
					item.Size = Math.Min(this.Size, nRemainingSize - item.Position);

					list.Add(item);
				}

				list.Add(tail);
			}
			catch
			{
				throw;
			}
		}
	}
}
