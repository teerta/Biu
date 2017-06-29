#pragma once


#include "bpeerdata.h"


using namespace System;
using namespace Biu;


namespace Biu { namespace Transmission {


	// Forwarding

	ref class Peer;


	// Ref Class DataContinuation

	public ref class DataContinuation
	{
	public:
		DataContinuation();

		/// <summary>
		/// ID of finished file.
		/// The is a sending or receving file id (by Direction).
		/// U need DataInfo.ConvertXXXToXXX methods for converting.
		/// </summary>
		property int ID;

		/// <summary>
		/// This is a sending or receving file.
		/// </summary>
		property bool Direction;
		property long long Size;
		property long long Origin;
		property long long FinishedSize;
		property String^ FilePath;
		property Peer^ PrimaryPeer;
		

	internal:
		void Create(BDataContinuation* pContinuation);
	};


	// Ref Class DataInfo

	public ref class DataInfo
	{
	public:
		DataInfo();


		property int ID
		{
			int __clrcall get() { return m_pInner->GetID(); }
		}

		property unsigned long long Size
		{
			unsigned long long __clrcall get() { return m_pInner->GetSize(); }
		}

		property bool Direction
		{
			bool __clrcall get() { return m_pInner->GetDirection(); }
		}

		property bool IsTransmitting
		{
			bool __clrcall get() { return m_pInner->IsTransmitting(); }
		}


		static int __clrcall ConvertToSendFileID(int recvFileID);
		static int __clrcall ConvertToRecvFileID(int sendFileID);

	internal:
		property BDataInfo* Inner
		{
			BDataInfo* get() { return m_pInner; }
			void set(BDataInfo* value) { m_pInner = value; }
		}

	private:
		BDataInfo*			m_pInner;
	};


	// Ref Class WaitDataInfo

	public ref class WaitDataInfo
	{
	public:
		WaitDataInfo();


		property String^ File
		{
			String^ get() { return gcnew String(m_pInner->GetFile()); }
		}

		property bool Direction
		{
			bool get() { return m_pInner->GetDirection(); }
		}


	internal:
		property BWaitDataInfo* Inner
		{
			BWaitDataInfo* get() { return m_pInner; }
			void set(BWaitDataInfo* value) { m_pInner = value; }
		}

	private:
		BWaitDataInfo*		m_pInner;
	};


	// Ref Class DataPeer

	public ref class DataPeer
	{
	public:
		DataPeer();

		DataInfo^ GetDataInfo(int nID);


		property int MaxSend
		{
			int get() { return m_pInner->GetMaxSend(); }
			void set(int value) { m_pInner->SetMaxSend(value); }
		}

		property int MaxRecv
		{
			int get() { return m_pInner->GetMaxRecv(); }
			void set(int value) { m_pInner->SetMaxRecv(value); }
		}

	internal:
		property BDataPeer* Inner
		{
			BDataPeer* get() { return m_pInner; }
			void set(BDataPeer* value) { m_pInner = value; }
		}

	private:
		BDataPeer*		m_pInner;
	};


	// Ref Class ClientDataPeer

	public ref class ClientDataPeer
	{
	public:
		ClientDataPeer();

		DataInfo^ GetDataInfo(int nID);


		property int MaxSend
		{
			int get() { return m_pInner->GetMaxSend(); }
			void set(int value) { m_pInner->SetMaxSend(value); }
		}

		property int MaxRecv
		{
			int get() { return m_pInner->GetMaxRecv(); }
			void set(int value) { m_pInner->SetMaxRecv(value); }
		}

	internal:
		property BClientDataPeer* Inner
		{
			BClientDataPeer* get() { return m_pInner; }
			void set(BClientDataPeer* value) { m_pInner = value; }
		}

	private:
		BClientDataPeer*	m_pInner;
	};


} }
