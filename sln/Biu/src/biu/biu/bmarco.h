
#ifndef _BMARCO_H_
#define _BMARCO_H_


#ifndef _WINDOWS

#ifndef __nullptr
#define __nullptr nullptr
#endif

#endif


#ifdef __cplusplus
	#define __VNULL	__nullptr
#else
	#define __VNULL	NULL
#endif


#ifdef _WINDOWS

#define VERIFY_HR(hr)					{ if (FAILED((HRESULT)(hr))) { return; } }
#define VERIFY_HR_RESULT(hr)			{ if (FAILED((HRESULT)(hr))) { return hr; } }
#define VERIFY_HR_FALSE(hr)				{ if (FAILED((HRESULT)(hr))) { return FALSE; } }
#define VERIFY_HR_false(hr)				{ if (FAILED((HRESULT)(hr))) { return false; } }

#define RETURN_HR(hr)					{ if (SUCCEEDED((HRESULT)(hr))){return TRUE;} else{return FALSE;} }
#define RETURN_hr(hr)					{ if (SUCCEEDED((HRESULT)(hr))){return true;} else{return false;} }

#define SAFE_RELEASE(x)					{ if (__VNULL != (x)) {(x)->Release(); (x) = __VNULL;} }
#define SAFE_ADDREF(x)					(((x) != __VNULL) ? ((x)->AddRef(), (x)) : __VNULL)
#define SAFE_CLOSE_HANDLE(h)			{ if (__VNULL != (h)){ CloseHandle((h)); } }
#define SAFE_CLOSE_HANDLE_NULL(h)		{ if (__VNULL != (h)){ CloseHandle((h)); (h) = __VNULL; } }
#define SAFE_CLOSE_HANDLE_INVALID(h)	{ if (INVALID_HANDLE_VALUE != (h)){ CloseHandle((h)); (h) = INVALID_HANDLE_VALUE; } }
#define SAFE_CLOSE_SOCKET(s)			{ if (INVALID_SOCKET != (s)){ closesocket((s)); (s) = INVALID_SOCKET; } }
#define CO_SAFE_DELETE(x)				{ if (__VNULL != (x)) { CoTaskMemFree((x)); (x) = __VNULL; } }
#define SAFE_SYSFREE_STRING(x)			{ if (__VNULL != (x)) { SysFreeString((x)); (x) = __VNULL; } }

#define CLOSE_HANDLE_NULL(h)			{ CloseHandle((h)); (h) = __VNULL; }
#define CLOSE_HANDLE_INVALID(h)			{ CloseHandle((h)); (h) = INVALID_HANDLE_VALUE; }

#define SAFE_SET_EVENT(h)				{ if (__VNULL != (h)) { SetEvent((h)); } }
#define SAFE_CLOSE_EVENT(h)				{ if (__VNULL != (h)) { SetEvent((h)); SAFE_CLOSE_HANDLE_NULL(h); } }

#define HEAP_ALLOC(s)					HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (s))
#define HEAP_FREE(p)					HeapFree(GetProcessHeap(), 0, (p))

#define LEAVE(s)						{ if (!(s)) { __leave; } }
#define LEAVE_INVALID(s)				{ if (INVALID_HANDLE_VALUE == (s)) { __leave; } }
#define LEAVE_HR(s)						{ if (FAILED((s))) { __leave; } }

#define CHK_FALSE(s)					if (!(s)) { return FALSE; }

#endif // _WINDOWS


#define SAFE_DELETE(x)					{ if (__VNULL != (x)) { delete (x); (x) = __VNULL; } }
#define SAFE_DELETE_ARRAY(x)			{ if (__VNULL != (x)) { delete[] (x); (x) = __VNULL; } }
#define SAFE_FREE(x)					{ if (__VNULL != (x)) { free((x)); (x) = __VNULL; } }

#define CHK(s)							if (!(s)) { return; }
#define CHK_NULL(s)						if (!(s)) { return __VNULL; }
#define CHK_N1(s)						if (!(s)) { return -1; }
#define CHK_false(s)					if (!(s)) { return false; }
#define CHK_0(s)						if (!(s)) { return 0; }
#define CHK_X(s, r)						if (!(s)) { return (X); }
#define CHK_THROW(p)                    if (!(p)) { throw; }
#define CHK_THROW_V(p, v)               if (!(p)) { throw (v); }
#define CHK_THROW_false(p)              if (!(p)) { throw false; }

#ifdef _WINDOWS
#define COPY_STRING_LEN(s, d, l)		{ SAFE_DELETE_ARRAY((d)); (l) = strlen((s)); (d) = new char[(l) + 1]; strcpy_s((d), (l) + 1, (s)); (d)[(l)] = '\0'; }
#else
#define COPY_STRING_LEN(s, d, l)		{ SAFE_DELETE_ARRAY((d)); (l) = strlen((s)); (d) = new char[(l) + 1]; strcpy((d), (s)); (d)[(l)] = '\0'; }
#endif

#define COPY_STRING(s, d)				{ int _x_sos = 0; COPYSTRINGLEN(s, d, _x_sos); }


#define MAKELONGLONG(l, h)      ((LONGLONG)(((DWORD)(((ULONGLONG)(l)) & 0xffffffff)) | ((LONGLONG)((DWORD)(((ULONGLONG)(h)) & 0xffffffff))) << 32))
#define LOLONG(l)           ((DWORD)(((ULONGLONG)(l)) & 0xffffffff))
#define HILONG(l)           ((DWORD)((((ULONGLONG)(l)) >> 32) & 0xffffffff))


#endif // _BMARCO_H_
