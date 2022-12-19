#include "error_msg_box.h"

// https://learn.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code

void errorMsgBox(LPWSTR funcName, LPWSTR desc) {
	// get last error message
	
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	
	// display error message
	
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCWSTR)lpMsgBuf) + lstrlen((LPCWSTR)funcName) + lstrlen((LPCWSTR)desc) + 40) * sizeof(WCHAR));
	StringCchPrintf((LPWSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(WCHAR),
		L"Error in %s at %s with code %d: %s",
		funcName, desc, dw, lpMsgBuf);
	MessageBox(NULL, (LPCWSTR)lpDisplayBuf, L"HyperMandel Error", MB_OK);
	
	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}
