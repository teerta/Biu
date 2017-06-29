
#include "Stdafx.h"
#include "Serializer.h"
#include "btype.h"


using namespace Biu;
using namespace Biu::Transmission;


// BiuSerializer

BiuSerializer::BiuSerializer()
{
}

BiuSerializer::~BiuSerializer()
{
}

BiuSerializer^ BiuSerializer::Serialize(System::Boolean val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::Char val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::Byte val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::SByte val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::Int16 val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::UInt16 val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::Int32 val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::UInt32 val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::Int64 val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::UInt64 val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::Single val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::Double val)
{
	Serializer->Serialize(val);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::String^ val)
{
	pin_ptr<const wchar_t> pval = PtrToStringChars(val);
	CAtlStringW str = pval;
	BCabUTF8String cab;
	cab = str;
	Serializer->Serialize(cab);
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(System::DateTime val)
{
	Serializer->Serialize(val.ToBinary());
	return this;
}

BiuSerializer^ BiuSerializer::Serialize(IBiuSerializable^ obj)
{
	obj->BSerialize(this);
	return this;
}

//BiuSerializer^ BiuSerializer::Serialize(array<String^>^ val)
//{
//	Serializer->Serialize(val->Length);
//
//	CAtlStringW sa;
//	BCabUTF8String cab;
//	for each (String^ str in val)
//	{
//		pin_ptr<const wchar_t> pstr = PtrToStringChars(str);
//		sa = pstr;
//		cab = sa;
//		Serializer->Serialize(cab);
//	}
//	return this;
//}

BiuSerializer^ BiuSerializer::Serialize(ICollection<String^>^ val)
{
	Serializer->Serialize(val->Count);

	CAtlStringW sa;
	BCabUTF8String cab;
	for each (String^ str in val)
	{
		pin_ptr<const wchar_t> pstr = PtrToStringChars(str);
		sa = pstr;
		cab = sa;
		Serializer->Serialize(cab);
	}
	return this;
}
			
generic<class K>
BiuSerializer^ BiuSerializer::Serialize(IDictionary<K, String^>^ val)
{
	Serializer->Serialize(val->Count);

	CAtlStringW sa;
	BCabUTF8String cab;
	for each (KeyValuePair<K, String^>^ pair in val)
	{
		pin_ptr<const wchar_t> pstr = PtrToStringChars(pair->Value);
		sa = pstr;
		cab = sa;
		Serializer->Serialize(cab);
	}
	return this;
}

//BiuSerializer^ BiuSerializer::Serialize(array<IBiuSerializable^>^ val)
//{
//	Serializer->Serialize(val->Length);
//
//	for each (IBiuSerializable^ obj in val)
//	{
//		obj->XXSerialize(this);
//	}
//
//	return this;
//}

generic<class T>
BiuSerializer^ BiuSerializer::Serialize(ICollection<T>^ val)
{
	Serializer->Serialize(val->Count);

	for each (T obj in val)
	{
		obj->BSerialize(this);
	}

	return this;
}
			
generic<class K, class V>
BiuSerializer^ BiuSerializer::Serialize(IDictionary<K, V>^ val)
{
	Serializer->Serialize(val->Count);

	for each (KeyValuePair<K, V>^ pair in val)
	{
		pair->Value->BSerialize(this);
	}

	return this;
}

//generic<class T>
//BiuSerializer^ BiuSerializer::SerializeT(array<T>^ val)
//{
//	Serializer->Serialize(val->Length);
//
//	Type^ t = T::typeid;
//	for each (T v in val)
//	{
//		SerializeV<T>(v, t);
//	}
//	return this;
//}

generic<class T>
BiuSerializer^ BiuSerializer::SerializeT(ICollection<T>^ val)
{
	Serializer->Serialize(val->Count);

	Type^ t = T::typeid;
	for each (T v in val)
	{
		SerializeV<T>(v, t);
	}
	return this;
}

generic<class K, class V>
BiuSerializer^ BiuSerializer::SerializeT(IDictionary<K, V>^ val)
{
	Serializer->Serialize(val->Count);

	Type^ t = V::typeid;
	for each (KeyValuePair<K, V>^ pair in val)
	{
		SerializeV<V>(pair->Value, t);
	}
	return this;
}

BiuSerializer^ BiuSerializer::SerializeBuffer(ICollection<byte>^ val)
{
	LPBYTE lpBuff = new BYTE[val->Count];
	int nIdx = 0;
	for each (byte b in val)
	{
		lpBuff[nIdx++] = b;
	}

	Serializer->Serialize(lpBuff, (unsigned int)val->Count);

	SAFE_DELETE_ARRAY(lpBuff);

	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Boolean% val)
{
	bool v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Char% val)
{
	wchar_t v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Byte% val)
{
	unsigned char v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::SByte% val)
{
	char v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Int16% val)
{
	short v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::UInt16% val)
{
	unsigned short v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Int32% val)
{
	int v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::UInt32% val)
{
	unsigned int v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Int64% val)
{
	long long v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::UInt64% val)
{
	unsigned long long v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Single% val)
{
	float v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::Double% val)
{
	double v;
	if (!Serializer->Deserialize(v))
	{
		throw gcnew BiuSerializeException();
	}

	val = v;
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::String^% val)
{
	BCabUTF8String cab;
	if (!Serializer->Deserialize(cab))
	{
		throw gcnew BiuSerializeException();
	}

	CStringW str;
	cab.ToUnicode(str);
	val = gcnew String(str);
	return this;
}

BiuSerializer^ BiuSerializer::Deserialize(System::DateTime% val)
{
	System::Int64 n;
	Deserialize(n);
	val = System::DateTime::FromBinary(n);
	return this;
}

generic<class T>
BiuSerializer^ BiuSerializer::Deserialize(T% obj)
{
	obj = (T)System::Activator::CreateInstance(T::typeid);
	obj->BDeserialize(this);
	return this;
}

//BiuSerializer^ BiuSerializer::Deserialize(array<String^>^% val)
//{
//	int nCount = 0;
//	Serializer->Deserialize(nCount);
//	if (!nCount)
//	{
//		return this;
//	}
//
//	CStringW str;
//	val = gcnew array<String^>(nCount);
//	for (int i = 0; i < nCount; i++)
//	{
//		BCabUTF8String cab;
//		if (!Serializer->Deserialize(cab))
//		{
//			throw gcnew BiuSerializeException();
//		}
//		cab.ToUnicode(str);
//		val[i] = gcnew String(str);
//	}
//	
//	return this;
//}

BiuSerializer^ BiuSerializer::Deserialize(ICollection<String^>^ val)
{
	int nCount = 0;
	Serializer->Deserialize(nCount);
	CStringW str;
	for (int i = 0; i < nCount; i++)
	{
		BCabUTF8String cab;
		if (!Serializer->Deserialize(cab))
		{
			throw gcnew BiuSerializeException();
		}
		cab.ToUnicode(str);
		val->Add(gcnew String(str));
	}
	
	return this;
}
			
generic<class K>
BiuSerializer^ BiuSerializer::Deserialize(DictionaryProxy<K, String^>^ proxy)
{
	int nCount = 0;
	Serializer->Deserialize(nCount);
	CStringW str;
	for (int i = 0; i < nCount; i++)
	{
		BCabUTF8String cab;
		if (!Serializer->Deserialize(cab))
		{
			throw gcnew BiuSerializeException();
		}
		cab.ToUnicode(str);
		String^ ss = gcnew String(str);
		proxy->MakeItem(ss);
	}
	
	return this;
}

//generic<class T>
//BiuSerializer^ BiuSerializer::Deserialize(array<T>^% val)
//{
//	int nCount = 0;
//	Serializer->Deserialize(nCount);
//	if (!nCount)
//	{
//		return this;
//	}
//	
//	val = gcnew array<T>(nCount);
//	try
//	{
//		for (int i = 0; i < nCount; i++)
//		{
//			T v = safe_cast<T>(System::Activator::CreateInstance(T::typeid));
//			v->XXDeserialize(this);
//			val[i] = v;
//		} 
//	}
//	catch (Exception^)
//	{
//		throw;
//	}
//
//	return this;
//}

generic<class T>
BiuSerializer^ BiuSerializer::Deserialize(ICollection<T>^ val)
{
	int nCount = 0;
	Serializer->Deserialize(nCount);
	try
	{
		for (int i = 0; i < nCount; i++)
		{
			T v = safe_cast<T>(System::Activator::CreateInstance(T::typeid));
			v->BDeserialize(this);
			val->Add(v);
		} 
	}
	catch (Exception^)
	{
		throw;
	}

	return this;
}
			
generic<class K, class V>
BiuSerializer^ BiuSerializer::Deserialize(DictionaryProxy<K, V>^ proxy)
{
	int nCount = 0;
	Serializer->Deserialize(nCount);
	try
	{
		for (int i = 0; i < nCount; i++)
		{
			V v = safe_cast<V>(System::Activator::CreateInstance(V::typeid));
			v->BDeserialize(this);
			proxy->MakeItem(v);
		} 
	}
	catch (Exception^)
	{
		throw;
	}

	return this;
}

//generic<class T>
//BiuSerializer^ BiuSerializer::DeserializeT(array<T>^% val)
//{
//	Type^ t = T::typeid;
//	int nCount = 0;
//	Serializer->Deserialize(nCount);
//	if (!nCount)
//	{
//		return this;
//	}
//
//	val = gcnew array<T>(nCount);
//
//	try
//	{
//		for (int i = 0; i < nCount; i++)
//		{		
//			val[i] = DeserializeV<T>(t);
//		}
//	}
//	catch(Exception^)
//	{
//		throw;
//	}
//	
//	return this;
//}

generic<class T>
BiuSerializer^ BiuSerializer::DeserializeT(ICollection<T>^ val)
{
	Type^ t = T::typeid;
	int nCount = 0;
	Serializer->Deserialize(nCount);
	try
	{
		for (int i = 0; i < nCount; i++)
		{		
			val->Add(DeserializeV<T>(t));
		}
	}
	catch(Exception^)
	{
		throw;
	}
	
	return this;
}

generic<class K, class V>
BiuSerializer^ BiuSerializer::DeserializeT(DictionaryProxy<K, V>^ proxy)
{
	Type^ t = V::typeid;
	int nCount = 0;
	Serializer->Deserialize(nCount);
	try
	{
		for (int i = 0; i < nCount; i++)
		{
			V v = DeserializeV<V>(t);
			proxy->MakeItem(v);
		}
	}
	catch(Exception^)
	{
		throw;
	}
	
	return this;
}

generic<class T>
void BiuSerializer::SerializeV(T v, Type^ t)
{
	if (bool::typeid == t)
	{
		this->Serializer->Serialize(static_cast<bool>(v));
	}
	else if (wchar_t::typeid == t)
	{
		this->Serializer->Serialize(static_cast<wchar_t>(v));
	}
	else if (char::typeid == t)
	{
		this->Serializer->Serialize(static_cast<char>(v));
	}
	else if (unsigned char::typeid == t)
	{
		this->Serializer->Serialize(static_cast<unsigned char>(v));
	}
	else if (signed char::typeid == t)
	{
		this->Serializer->Serialize(static_cast<signed char>(v));
	}
	else if (short::typeid == t)
	{
		this->Serializer->Serialize(static_cast<short>(v));
	}
	else if (unsigned short::typeid == t)
	{
		this->Serializer->Serialize(static_cast<unsigned short>(v));
	}
	else if (int::typeid == t)
	{
		this->Serializer->Serialize(static_cast<int>(v));
	}
	else if (unsigned int::typeid == t)
	{
		this->Serializer->Serialize(static_cast<unsigned int>(v));
	}
	else if (long long::typeid == t)
	{
		this->Serializer->Serialize(static_cast<long long>(v));
	}
	else if (unsigned long long::typeid == t)
	{
		this->Serializer->Serialize(static_cast<unsigned long long>(v));
	}
	else if (float::typeid == t)
	{
		this->Serializer->Serialize(static_cast<float>(v));
	}
	else if (double::typeid == t)
	{
		this->Serializer->Serialize(static_cast<double>(v));
	}
	//else if (String::typeid == t)
	//{
	//	Serialize(safe_cast<String^>(v));
	//}
	//else if (safe_cast<IBiuSerializable^>(v))
	//{
	//	Serialize(safe_cast<IBiuSerializable^>(v));
	//}
}

generic<class T>
T BiuSerializer::DeserializeV(Type^ t)
{
	bool bRet = false;
	T v;
	if (bool::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<bool&>(v));
	}
	else if (wchar_t::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<wchar_t&>(v));
	}
	else if (char::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<char&>(v));
	}
	else if (unsigned char::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<unsigned char&>(v));
	}
	else if (signed char::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<char&>(v));
	}
	else if (short::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<short&>(v));
	}
	else if (unsigned short::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<unsigned short&>(v));
	}
	else if (int::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<int&>(v));
	}
	else if (unsigned int::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<unsigned int&>(v));
	}
	else if (long long::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<long long&>(v));
	}
	else if (unsigned long long::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<unsigned long long&>(v));
	}
	else if (float::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<float&>(v));
	}
	else if (double::typeid == t)
	{
		bRet = this->Serializer->Deserialize(reinterpret_cast<double&>(v));
	}
	//else if (String::typeid == t)
	//{
	//	try
	//	{
	//		Deserialize(safe_cast<String^>(v));
	//		bRet = true;
	//	}
	//	catch(Exception^)
	//	{
	//		bRet = false;
	//	}
	//}
	//else if (safe_cast<IBiuSerializable^>(v))
	//{
	//	try
	//	{
	//		Deserialize(safe_cast<IBiuSerializable^>(v));
	//		bRet = true;
	//	}
	//	catch(Exception^)
	//	{
	//		bRet = false;
	//	}
	//}

	if (!bRet)
	{
		throw gcnew BiuSerializeException();
	}

	return v;
}

BiuSerializer^ BiuSerializer::DeserializeBuffer(ICollection<byte>^ val)
{
	LPBYTE lpBuff = NULL;
	unsigned int cb = 0;
	if (!Serializer->Deserialize(&lpBuff, &cb))
	{
		throw gcnew BiuSerializeException();
	}

	val->Clear();
	for (unsigned int i = 0; i < cb; i++)
	{
		val->Add(lpBuff[i]);
	}

	SAFE_DELETE_ARRAY(lpBuff);

	return this;
}
