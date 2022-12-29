#pragma once

#include "error_msg_box.h"

// checks function return value is zero indicating it errored, if so opens a MsgBox, with optional extra action version
#define ERROR_CHECK_ZERO(returnVal, funcName, location) {\
	if (!returnVal) {\
		WCHAR _funcName[] = funcName;\
		WCHAR _location[] = location;\
		errorMsgBox(_funcName, _location);\
	}\
}
#define ERROR_CHECK_ZERO_EXTRA(returnVal, funcName, location, extraAction) {\
	if (!returnVal) {\
		WCHAR _funcName[] = funcName;\
		WCHAR _location[] = location;\
		errorMsgBox(_funcName, _location);\
		extraAction;\
	}\
}

// checks function return value is nonzero indicating it errored, if so opens a MsgBox, with optional extra action version
#define ERROR_CHECK_NONZERO(returnVal, funcName, location) {\
	if (returnVal) {\
		WCHAR _funcName[] = funcName;\
		WCHAR _location[] = location;\
		errorMsgBox(_funcName, _location);\
	}\
}
#define ERROR_CHECK_NONZERO_EXTRA(returnVal, funcName, location, extraAction) {\
	if (returnVal) {\
		WCHAR _funcName[] = funcName;\
		WCHAR _location[] = location;\
		errorMsgBox(_funcName, _location);\
		extraAction;\
	}\
}

// wraps Win32 API call in value check if it isn't zero, if so prints debug warning
#ifdef _DEBUG
#define WARN_WRAP_NONZERO(funcCall, funcName, location, msg) {\
	BOOL _result = funcCall;\
	if (_result) {\
		PRINT_DEBUG_NOARG(L"Runtime Warning in " funcName L" at " location L": " msg);\
	}\
}
#else
#define WARN_WRAP_NONZERO(funcCall, funcName, location) funcCall;
#endif

// wraps Win32 API call in value check if it is zero, if so prints debug warning
#ifdef _DEBUG
#define WARN_WRAP_ZERO(funcCall, funcName, location, msg) {\
	BOOL _result = funcCall;\
	if (!_result) {\
		PRINT_DEBUG_NOARG(L"Runtime Warning in " funcName L" at " location L": " msg);\
	}\
}
#else
#define WARN_WRAP_ZERO(funcCall, funcName, location) funcCall;
#endif

#define PRINT_DEBUG_NOARG(msg) OutputDebugString(msg L"\n")

#define PRINT_DEBUG_2ARG(label, arg1, arg2) {\
	std::wostringstream debugOutputStream;\
	\
	debugOutputStream << label;\
	debugOutputStream << arg1;\
	debugOutputStream << L", ";\
	debugOutputStream << arg2;\
	debugOutputStream << L"\n";\
	\
	std::wstring debugOutput = debugOutputStream.str();\
	\
	OutputDebugString(debugOutput.c_str());\
}

#define PRINT_DEBUG_4ARG(label, arg1, arg2, arg3, arg4) {\
	std::wostringstream debugOutputStream;\
	\
	debugOutputStream << label;\
	debugOutputStream << arg1;\
	debugOutputStream << L", ";\
	debugOutputStream << arg2;\
	debugOutputStream << L", ";\
	debugOutputStream << arg3;\
	debugOutputStream << L", ";\
	debugOutputStream << arg4;\
	debugOutputStream << L"\n";\
	\
	std::wstring debugOutput = debugOutputStream.str();\
	\
	OutputDebugString(debugOutput.c_str());\
}
