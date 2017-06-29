
#pragma once


#include "bs.h"


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace Biu;


namespace Biu { namespace Transmission {


		// Forwarding Declaration

		ref class BiuSerializer;


		// Delegates

		generic<class K, class V>
		public delegate K KeyForValue(V val);

		
		// Interface Class IBiuSerializable

		public interface class IBiuSerializable
		{
			bool __clrcall BSerialize(BiuSerializer^ ser);
			bool __clrcall BDeserialize(BiuSerializer^ ser);
		};


		// Ref Class BSerializeException

		public ref class BiuSerializeException : public Exception
		{
		public:
			__clrcall BiuSerializeException()
				: Exception()
			{
			}

			__clrcall BiuSerializeException(String^ message)
				: Exception(message)
			{
			}

			__clrcall ~BiuSerializeException()
			{
			}
		};


		// Ref Class DictionaryProxy

		generic<class K, class V>
		public ref class DictionaryProxy sealed
		{
		public:
			__clrcall DictionaryProxy(IDictionary<K, V>^ dict, KeyForValue<K, V>^ func)
			{
				this->Dictionary = dict;
				this->KeyForValue = func;
			}

			void __clrcall MakeItem(V val)
			{
				this->Dictionary[this->KeyForValue(val)] = val;
			}

			property IDictionary<K, V>^ Dictionary;
			property KeyForValue<K, V>^ KeyForValue;
		};


		// Ref Class BiuSerializer

		public ref class BiuSerializer sealed
		{
		public:
			BiuSerializer();
			~BiuSerializer();

			BiuSerializer^ __clrcall Serialize(System::Boolean val);
			BiuSerializer^ __clrcall Serialize(System::Char val);
			BiuSerializer^ __clrcall Serialize(System::Byte val);
			BiuSerializer^ __clrcall Serialize(System::SByte val);
			BiuSerializer^ __clrcall Serialize(System::Int16 val);
			BiuSerializer^ __clrcall Serialize(System::UInt16 val);
			BiuSerializer^ __clrcall Serialize(System::Int32 val);
			BiuSerializer^ __clrcall Serialize(System::UInt32 val);
			BiuSerializer^ __clrcall Serialize(System::Int64 val);
			BiuSerializer^ __clrcall Serialize(System::UInt64 val);
			BiuSerializer^ __clrcall Serialize(System::Single val);
			BiuSerializer^ __clrcall Serialize(System::Double val);
			BiuSerializer^ __clrcall Serialize(System::String^ val);
			BiuSerializer^ __clrcall Serialize(System::DateTime val);
			BiuSerializer^ __clrcall Serialize(IBiuSerializable^ obj);

			//BiuSerializer^ __clrcall Serialize(array<String^>^ val);
			BiuSerializer^ __clrcall Serialize(ICollection<String^>^ val);

			generic<class K>
			BiuSerializer^ __clrcall Serialize(IDictionary<K, String^>^ val);

			//BiuSerializer^ __clrcall Serialize(array<IXXSerializable^>^ val);

			generic<class T>
			where T : IBiuSerializable
			BiuSerializer^ __clrcall Serialize(ICollection<T>^ val);

			generic<class K, class V>
				where V : IBiuSerializable
			BiuSerializer^ __clrcall Serialize(IDictionary<K, V>^ val);

			//generic<class T>
			//where T : value class
			//BiuSerializer^ __clrcall SerializeT(array<T>^ val);

			generic<class T>
			where T : value class
			BiuSerializer^ __clrcall SerializeT(ICollection<T>^ val);

			generic<class K, class V>
			where V : value class
			BiuSerializer^ __clrcall SerializeT(IDictionary<K, V>^ val);

			BiuSerializer^ __clrcall SerializeBuffer(ICollection<byte>^ val);


			BiuSerializer^ __clrcall Deserialize(System::Boolean% val);
			BiuSerializer^ __clrcall Deserialize(System::Char% val);
			BiuSerializer^ __clrcall Deserialize(System::Byte% val);
			BiuSerializer^ __clrcall Deserialize(System::SByte% val);
			BiuSerializer^ __clrcall Deserialize(System::Int16% val);
			BiuSerializer^ __clrcall Deserialize(System::UInt16% val);
			BiuSerializer^ __clrcall Deserialize(System::Int32% val);
			BiuSerializer^ __clrcall Deserialize(System::UInt32% val);
			BiuSerializer^ __clrcall Deserialize(System::Int64% val);
			BiuSerializer^ __clrcall Deserialize(System::UInt64% val);
			BiuSerializer^ __clrcall Deserialize(System::Single% val);
			BiuSerializer^ __clrcall Deserialize(System::Double% val);
			BiuSerializer^ __clrcall Deserialize(System::String^% val);
			BiuSerializer^ __clrcall Deserialize(System::DateTime% val);

			generic<class T>
			where T : IBiuSerializable
			BiuSerializer^ __clrcall Deserialize(T% obj);

			//BiuSerializer^ __clrcall Deserialize(array<String^>^% val);
			BiuSerializer^ __clrcall Deserialize(ICollection<String^>^ val);

			generic<class K>
			BiuSerializer^ __clrcall Deserialize(DictionaryProxy<K, String^>^ proxy);

			//generic<class T>
			//where T : IXXSerializable
			//BiuSerializer^ __clrcall Deserialize(array<T>^% val);

			generic<class T>
			where T : IBiuSerializable
			BiuSerializer^ __clrcall Deserialize(ICollection<T>^ val);

			generic<class K, class V>
			where V : IBiuSerializable
			BiuSerializer^ __clrcall Deserialize(DictionaryProxy<K, V>^ proxy);

			//generic<class T>
			//where T : value class
			//BiuSerializer^ __clrcall DeserializeT(array<T>^% val);

			generic<class T>
			where T : value class
			BiuSerializer^ __clrcall DeserializeT(ICollection<T>^ val);

			generic<class K, class V>
			where V : value class
			BiuSerializer^ __clrcall DeserializeT(DictionaryProxy<K, V>^ proxy);

			BiuSerializer^ __clrcall DeserializeBuffer(ICollection<byte>^ val);

		internal:
			property BSerializer* Serializer;

		private:
			generic<class T>
			where T : value class
			void __clrcall SerializeV(T v, Type^ t);

			generic<class T>
			where T : value class
			T __clrcall DeserializeV(Type^ t);
		};


} }
