#pragma once

#include "error_msg_box.h"

// wraps Win32 API call in a MsgBox indicating it errored.
#define ERROR_WRAP(funcCall, funcName, desc) {\
	BOOL _result = funcCall;\
	if (!_result) {\
		WCHAR _funcName[] = funcName;\
		WCHAR _desc[] = desc;\
		errorMsgBox(_funcName, _desc);\
	}\
}
