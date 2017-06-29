
#include "stdafx.h"
#include "AES.h"
#include "CryptKey.h"
#include "CryptProvider.h"


using namespace Biu::Native::Security;
using namespace System::Runtime::InteropServices;
using namespace System::Linq;


#define ENCRYPT_BLOCK_LEN				8


AES::AES()
	: m_dwBatch(8192)
{

}

AES::~AES()
{

}


void AES::EncryptStream(Stream^ src, Stream^ dest, CryptKey^ key, unsigned int size, cli::array<Byte>^% hash)
{
	hash = nullptr;

	HCRYPTHASH hHash = NULL;
	LPBYTE lpBatchBuff = NULL;
	DWORD cbBatch = 0;
	DWORD cbBatchBuff = 0;	
	cli::array<Byte>^ buff = nullptr;

	try
	{
		if (!CryptCreateHash(key->m_pProv->GetProvider(), CALG_SHA_256, 0, 0, &hHash))
		{
			throw gcnew Exception(_T("Failed: Create hash"));
		}

		cbBatch = m_dwBatch - m_dwBatch % ENCRYPT_BLOCK_LEN;
		if (ENCRYPT_BLOCK_LEN > 1)
		{
			cbBatchBuff = cbBatch + ENCRYPT_BLOCK_LEN;
		}
		else
		{
			cbBatchBuff = cbBatch;
		}

		lpBatchBuff = new BYTE[cbBatchBuff];
		IntPtr ptr(lpBatchBuff);
		buff = gcnew cli::array<Byte>(cbBatchBuff);

		//src->Seek(0ll, SeekOrigin::Begin);

		int cbTotal = 0 != size ? size : src->Length - src->Position;
		DWORD cb = 0;
		int nRead = 0;
		int nBatch = 0;
		BOOL bEOS = FALSE;
		do
		{
			nBatch = Math::Min(cbTotal - nRead, cbBatch);
			cb = src->Read(buff, 0, nBatch);
			nRead += cb;
			if (nRead >= cbTotal)
			{
				bEOS = TRUE;
			}

			Marshal::Copy(buff, 0, ptr, cb);
			if (!CryptEncrypt(key->m_hKey, hHash, bEOS, 0, lpBatchBuff, &cb, cbBatchBuff))
			{
				throw gcnew Exception(_T("Failed: encrypt"));
			}

			Marshal::Copy(ptr, buff, 0, cb);
			dest->Write(buff, 0, cb);
		} while (!bEOS);

		DWORD cbDataSize = sizeof(DWORD);
		if (!CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&cb, &cbDataSize, 0))
		{
			throw gcnew Exception(_T("Failed: Get hash size"));
		}

		if (!CryptGetHashParam(hHash, HP_HASHVAL, lpBatchBuff, &cb, 0))
		{
			throw gcnew Exception(_T("Failed: Get hash"));
		}

		hash = gcnew cli::array<Byte>(cb);
		Marshal::Copy(ptr, hash, 0, cb);
	}
	catch (...)
	{
		throw;
	}
	finally
	{
		if (nullptr != buff)
		{
			buff = nullptr;
		}

		SAFE_DELETE_ARRAY(lpBatchBuff);

		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}
	}
}


void AES::DecryptStream(Stream^ src, Stream^ dest, CryptKey^ key, unsigned int size, IEnumerable<Byte>^ hash)
{
	if (nullptr == hash)
	{
		throw gcnew ArgumentNullException(_T("hash"));
	}

	HCRYPTHASH hHash = NULL;
	LPBYTE lpBatchBuff = NULL;
	DWORD cbBatch = 0;
	DWORD cbBatchBuff = 0;	
	cli::array<Byte>^ buff = nullptr;

	try
	{
		if (!CryptCreateHash(key->m_pProv->GetProvider(), CALG_SHA_256, 0, 0, &hHash))
		{
			throw gcnew Exception(_T("Failed: Create hash"));
		}

		cbBatch = m_dwBatch - m_dwBatch % ENCRYPT_BLOCK_LEN;
		if (ENCRYPT_BLOCK_LEN > 1)
		{
			cbBatchBuff = cbBatch + ENCRYPT_BLOCK_LEN;
		}
		else
		{
			cbBatchBuff = cbBatch;
		}

		lpBatchBuff = new BYTE[cbBatchBuff];
		IntPtr ptr(lpBatchBuff);
		buff = gcnew cli::array<Byte>(cbBatchBuff);

		int cbTotal = 0 != size ? size : src->Length - src->Position;
		DWORD cb = 0;
		int nRead = 0;
		int nBatch = 0;
		BOOL bEOS = FALSE;
		do
		{
			nBatch = Math::Min(cbTotal - nRead, cbBatch);
			cb = src->Read(buff, 0, nBatch);
			nRead += cb;
			if (nRead >= cbTotal)
			{
				bEOS = TRUE;
			}

			Marshal::Copy(buff, 0, ptr, cb);
			if (!CryptDecrypt(key->m_hKey, hHash, bEOS, 0, lpBatchBuff, &cb))
			{
				throw gcnew Exception(_T("Failed: decrypt"));
			}

			Marshal::Copy(ptr, buff, 0, cb);
			dest->Write(buff, 0, cb);
		} while (!bEOS);

		DWORD cbDataSize = sizeof(DWORD);
		if (!CryptGetHashParam(hHash, HP_HASHSIZE, (LPBYTE)&cb, &cbDataSize, 0))
		{
			throw gcnew Exception(_T("Failed: Get hash size"));
		}

		if (!CryptGetHashParam(hHash, HP_HASHVAL, lpBatchBuff, &cb, 0))
		{
			throw gcnew Exception(_T("Failed: Get hash"));
		}

		if (cb != Enumerable::Count(hash))
		{
			throw gcnew Exception(_T("Failed: Verify hash length"));
		}

		for (DWORD i = 0; i < cb; i++)
		{
			if (lpBatchBuff[i] != Enumerable::ElementAt(hash, (int)i))
			{
				throw gcnew Exception(_T("Failed: Verify hash"));
			}
		}
	}
	catch (...)
	{
		throw;
	}
	finally
	{
		if (nullptr != buff)
		{
			buff = nullptr;
		}

		SAFE_DELETE_ARRAY(lpBatchBuff);

		if (hHash)
		{
			CryptDestroyHash(hHash);
			hHash = NULL;
		}
	}
}
