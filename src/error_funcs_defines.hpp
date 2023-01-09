#pragma once

#include "error_msg_box.hpp"

#define errorMsgBox_const(funcName, location) {\
	WCHAR _funcName[] = funcName;\
	WCHAR _location[] = location;\
	errorMsgBox(_funcName, _location);\
}

// checks function return value is zero indicating it errored, if so opens a MsgBox, with optional extra action version
#define ERROR_CHECK_ZERO(returnVal, funcName, location) {\
	if (!returnVal) {\
		errorMsgBox_const(funcName, location);\
	}\
}
#define ERROR_CHECK_ZERO_EXTRA(returnVal, funcName, location, extraAction) {\
	if (!returnVal) {\
		errorMsgBox_const(funcName, location);\
		extraAction;\
	}\
}

// checks function return value is nonzero indicating it errored, if so opens a MsgBox, with optional extra action version
#define ERROR_CHECK_NONZERO(returnVal, funcName, location) {\
	if (returnVal) {\
		errorMsgBox_const(funcName, location);\
	}\
}
#define ERROR_CHECK_NONZERO_EXTRA(returnVal, funcName, location, extraAction) {\
	if (returnVal) {\
		errorMsgBox_const(funcName, location);\
		extraAction;\
	}\
}

// wraps Win32 API call in value check if it isn't zero, if so prints debug warning
#ifdef _DEBUG
#define WARN_CHECK_NONZERO(returnVal, funcName, location, msg) {\
	if (returnVal) {\
		PRINT_DEBUG_NOARG(L"Runtime Warning in " funcName L" at " location L": " msg);\
	}\
}
#else
#define WARN_CHECK_NONZERO(returnVal, funcName, location, msg) returnVal;
#endif

// wraps Win32 API call in value check if it is zero, if so prints debug warning
#ifdef _DEBUG
#define WARN_CHECK_ZERO(returnVal, funcName, location, msg) {\
	if (!returnVal) {\
		PRINT_DEBUG_NOARG(L"Runtime Warning in " funcName L" at " location L": " msg);\
	}\
}
#else
#define WARN_CHECK_ZERO(returnVal, funcName, location, msg) returnVal;
#endif
