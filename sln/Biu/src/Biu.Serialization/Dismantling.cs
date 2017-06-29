using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Security.Cryptography;
using Biu.Native.Security;

namespace Biu.Serialization
{
	/// <summary>
	/// Dismantling is a subclass of PersistenceBase that split data into parts.
	/// </summary>
	public class Dismantling : PersistenceBase
	{
		/// <summary>
		/// Default buffer size is 4096.
		/// </summary>
		public int BufferSize { get; set; }

		/// <summary>
		/// Method that encrypts and decrypts data.
		/// </summary>
		public IStreamTransform Transform { get; set; }
		
		/// <summary>
		/// Caller should set this property before do somethine else.
		/// </summary>
		public PersistencePlan Plan { get; set; }


		public Dismantling()
		{
			this.BufferSize = 4096;
		}


		#region PersistenceBase
		public override PersistentResult Save(Stream stream, Func<PersistentItem, string> nameItemFile = null)
		{
			SHA256Managed sha256 = null;
			var result = new PersistentResult();
			try
			{
				var partitions = this.Plan.Partition(stream);
				for (int i = 0; i < partitions.Count(); i++)
				{                    
					var par = partitions.ElementAt(i);
					var decision = this.Plan.MakeSavingDecision(par);
					var item = new PersistentItem()
					{
						ItemType = par.ItemType,
						StoredType = decision.StoredType,
						Significance = decision.Significance,
						Order = i
					};

					item.File = null == nameItemFile ? null : nameItemFile(item);
					item.CreateMemoryStream();

					if (null == decision.Key)
					{
						item.IsEncrypted = false;

						var buff = new byte[this.BufferSize];

						int nRemaining = (int)par.Size;
						int nRead = 0;
						do
						{
							nRead = stream.Read(buff, 0, Math.Min(this.BufferSize, nRemaining));
							nRemaining -= nRead;
							if (0 != nRead)
							{
								item.Stream.Write(buff, 0, nRead);
							}
						} while (0 != nRemaining);

						if (null == sha256)
						{
							sha256 = new SHA256Managed();
						}

						long nPos = item.Stream.Position;
						item.Stream.Seek(0L, SeekOrigin.Begin);
						item.Hash = sha256.ComputeHash(item.Stream);
						item.Stream.Seek(nPos, SeekOrigin.Begin);
					}
					else
					{
						item.IsEncrypted = true;

						byte[] hash = null;
						this.Transform.EncryptStream(stream, item.Stream, decision.Key, (uint)par.Size, ref hash);
						item.Hash = hash;
					}

					item.EnsureData();
					if (null != decision.Formatter)
					{
						decision.Formatter.Serialize(item);
					}

					if (decision.CloseStreamWhenDone)
					{
						item.CloseStream();
					}

					result.Items.Add(item);
				}

				result.IsSucceeded = true;
			}
			catch
			{
				throw;
			}
			finally
			{
				if (null != sha256)
				{
					sha256.Dispose();
					sha256 = null;
				}
			}

			return result;
		}

		public override Stream Load(IEnumerable<PersistentItem> items)
		{			
			bool bSucc = false;
			Stream stream = null;
			Stream streamData = null;
			long nOffset = 0L;

			try
			{
				stream = new MemoryStream();

				foreach (var item in items.OrderBy(i => i.Order))
				{
					var decision = this.Plan.MakeLoadingDecision(item);
					if (null == item)
					{
						throw new PersistenceException("No decision for PersistentItem");
					}

					if (item.NeedSerialization)
					{
						if (null == decision.Formatter)
						{
							throw new PersistenceException("PersistentItem needs a formatter");
						}

						decision.Formatter.Deserialize(item, out nOffset);
					}

					switch (item.StoredType)
					{
						case PersistentItemStoredType.Binary:
							streamData = new MemoryStream();
							streamData.Write(item.Binary.ToArray(), 0, item.Binary.Count());
							streamData.Seek(0, SeekOrigin.Begin);
							break;

						case PersistentItemStoredType.Stream:
							streamData = item.Stream;
							streamData.Seek(0, SeekOrigin.Begin);
							break;

						case PersistentItemStoredType.File:
							streamData = new FileStream(item.File, FileMode.Open, FileAccess.Read, FileShare.Write);
							streamData.Seek(nOffset, SeekOrigin.Begin);
							nOffset = 0L;
							break;
					}
					
					if (item.IsEncrypted)
					{
						if (null == decision.Key)
						{
							throw new PersistenceException("PersistentItem needs a key");
						}

						this.Transform.DecryptStream(streamData, stream, decision.Key, 0, item.Hash);
					}
					else
					{
						var buff = new byte[this.BufferSize];

						int cb = 0;
						do
						{
							cb = streamData.Read(buff, 0, this.BufferSize);
							if (0 != cb)
							{
								stream.Write(buff, 0, cb);
							}
						} while (0 != cb);
					}

					if (item.StoredType != PersistentItemStoredType.Stream)
					{
						streamData.Close();
						streamData = null;
					}
				}

				bSucc = true;
			}
			catch
			{
				throw;
			}
			finally
			{
				if (null != streamData)
				{
					streamData.Close();
					streamData = null;
				}

				if (!bSucc)
				{
					if (null != stream)
					{
						stream.Close();
						stream = null;
					}
				}
			}

			return stream;
		}
		#endregion		


		/// <summary>
		/// Get the 1st formatter in formatter collection.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <returns></returns>
		public T GetFirstFormatter<T>()
			where T : class, IPersistentItemFormatter
		{
			return this.Plan.Formatters.FirstOrDefault() as T;
		}

		/// <summary>
		/// Get the 1st key in key collection.
		/// </summary>
		/// <returns></returns>
		public CryptKey GetFirstKey()
		{
			return this.Plan.Keys.FirstOrDefault();
		}
	}
}
